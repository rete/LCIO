
# FIO design document


## Goal

The current I/O layer of the LCIO library (SIO) suffers from many problems:

- **Global variables**: The `SIO_blockManager`, `SIO_streamManager` and `SIO_recordManager` APIs are all static member functions. When using a `LCReader` and `LCWriter` in the same program, the global memory has to be cleaned-up each time a 'read' or a 'write' is performed. This involves un-necessary allocation and deletion of memory.
- **Thread safety**: Direct consequence from global variables design, SIO is not thread-safe and thus can't be used in a multithreaded environement.
- **Lack of flexibility**: In the current design, events (LCEvent) strcture, or 'records' in SIO jargon, can only be written in a regular file. A possibility to use other mechanism such as in-memory buffer or sockets may be necessary for DAQ systems. A flexible library should allow for this.
- **Separation of concerns**: The SIO design does not really apply separation of concerns principle, e.g, the SIO_stream class handle the file description and operation AND record buffer operations such as compression/decompression. The SIO library is also not granular and thread safe enough to apply implicit multithreading in read and write operation on record or blocks.
- **Old C style**: The SIO library is written in old C style and design. Using modern C++ 11/14 would improve maintenance, I/O speed and allow for a better long term support.

Considering these points, a new I/O implementation is investigated. **I/O speedup** is the main motivation. Achieving better I/O performances would descrease reconstruction and analysis CPU time.

Also, the new implementation must be fully backward AND forward compatible regarding the final serialized file content. Old LCIO files must be readable by the new implementation and new ones must readable using the old implementation.


## From Simple IO (SIO) to Fast IO (FIO)

### Data structure

The current data structure is composed of the following components:

- a *record*: a composite data structure made of a header and a list of *blocks* (see below). The record header contains the following information:
   - the length of the record header: *unsigned int* (4 bytes)
   - a record marker: *unsigned int* (4 bytes), value=`0xabadcafe`
   - an option word storing low level and user defined information on the record: *unsigned int* (4 bytes)
   - the length of the compressed record buffer: *unsigned int* (4 bytes)
   - the length of the uncompressed record buffer: *unsigned int* (4 bytes)
   - the length of the record name: *unsigned int* (4 bytes)
   - the record name: *char array*
- a *block*: the base buffer unit in a record. The read and writing of a block is user defined and convert data from or to binary using the streaming facility. In memory, a block is composed of header containing metadata obout the block itself and the block data themselves. The block is composed of:
   - The length of the block header: *unsigned int* (4 bytes)
   - The block marker: *unsigned int* (4 bytes), value=`0xdeadbeef`
   - The block user version: *unsigned int* (4 bytes). This allows for schema evolution in the event data model and to keep backward compatibility on read.
   - The length of the block name: *unsigned int* (4 bytes)
   - The block name: *char array*

The SIO library provides a mechanism to write referenced pointer to the file without duplicated the structure streaming (so called pointed at and pointer to). On read operation, when the record has been fully read, the referenced pointers are relocated using of map of old address VS new address. This mechanism is refered as *pointer relocation* later on.

The data structure as description above must not change. In contrary, what can be changed is the way we perform read and write operations (We can change HOW but not WHAT).

### Architecture (re)consideration

1) Read operation

To parallelize the read operation, one has to find out where could be applied. A natural way to read data in parallel is to read concurrently block data in a record, as each block buffer reading is independant. The following strategy could be applied:

- extract the record buffer from the file
- uncompress the record buffer using zlib
- delimit blocks (LCCollection) begin and end in the buffer (thanks to the block header)
- **run block read operation in parallel (using async call)**
- collect block read products (join operation)
- run pointer relocation for all read blocks
- merge block result into final record product (LCEvent)

Note that this parallelism is implicit for the user and does not require any multithreading knowledge to use it.

2) Write operation

As for the read operation, parallelism could be applied to the block write operation:

- setup a buffer in write mode for each block
- **run block write operation in parallel (using async call)**
- collect buffers from different blocks and merge them in a final one. Note that this operation
   - can be done in parallel as soon as a block buffer writing is finished
   - is necessary in order to compress a single buffer using zlib
- compress the final record buffer
- write record header in the file
- write compressed record buffer in the file


3) Note on the implementation design

Separation of concerns and constness are the key ingredients of this implementation. By encapsulating the block data reading/writing correctly, the implementation might be fully lock-free.
