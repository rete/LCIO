

#include <FIOBuffer.h>

namespace fio {

  /// The standard std::memcpy call
  void standard_copy::memcpy(
    buffer_type*       destination,
    const buffer_type* source,
    size_type          size,
    size_type          count) {
    std::memcpy(destination, source, count*size);
  }

  /// Copy data to 'destination' in big endian
  void big_endian_copy::memcpy(
    buffer_type*       destination,
    const buffer_type* source,
    size_type          size,
    size_type          count) {
#ifdef FIO_LITTLE_ENDIAN
    destination += size;
    for(size_type icnt = 0 ; icnt<count ; icnt++) {
      for(size_type ibyt = 0 ; ibyt<size ; ibyt++) {
        *--destination = *source++;
      }
      destination += (size << 1);
    }
#else
    memcpy(destination, source, size, count);
#endif
  }

  /// Copy data to 'destination' in little endian
  void little_endian_copy::memcpy(
    buffer_type*       destination,
    const buffer_type* source,
    size_type          size,
    size_type          count) {
#ifdef FIO_LITTLE_ENDIAN
    memcpy(destination, source, size, count);
#else
    destination += size;
    for(size_type icnt = 0 ; icnt<count ; icnt++) {
      for(size_type ibyt = 0 ; ibyt<size ; ibyt++) {
        *--destination = *source++;
      }
      destination += (size << 1);
    }
#endif
  }
}
