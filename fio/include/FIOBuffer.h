
#ifndef FIO_BUFFER_H
#define FIO_BUFFER_H 1

// -- std headers
#include <memory>
#include <cstring>
#include <numeric_limits>
#include <iostream>

namespace fio {
  
  template < class charT, class alloc = std::allocate<charT>>
  class buffer {
    buffer() = delete;
    buffer(const buffer&) = delete;
    buffer& operator=(const buffer&) = delete;
    
  public:
    // traits
    typedef charT                   char_type;
    typedef traits                  traits_type;
    typedef alloc                   allocator_type;
    typedef int                     int_type;
    typedef std::size_t             pos_type;
    typedef long                    off_type;
    typedef std::size_t             size_type;
    typedef std::ios_base::seekdir  seek_dir;
    typedef std::ios_base::openmode open_mode;
    typedef std::ios_base::iostate  io_state;
    // constants
    static constexpr size_type      default_size = 2*1024*1024; // 2 Mo to start ...
    static constexpr size_type      default_expand = 1024*1024; // expanding
    static constexpr size_type      max_size = std::numeric_limits<size_type>::max() / sizeof(value_type);
    
  public:
    /// Allocate a buffer in write mode
    buffer(size_type size = default_size) {
      allocator_type allocator;
      m_buffer = allocator.allocate(size);
      m_size = size;
      m_memsize = size;
      std::fill(m_buffer, m_buffer+m_size, char_type{});
      m_current = m_buffer;
      m_mode = std::ios_base::out | std::ios_base::binary;
    }
    
    /// Adopt/copy the buffer and set the buffer in read mode
    buffer(char_type *&bytes, size_type len, bool copy = false) {
      if(nullptr == bytes) {
        setstate(std::ios_base::badbit);
        return;
      }
      if(copy) {
        allocator_type allocator;
        m_buffer = allocator.allocate(size);
        std::memcpy(m_buffer, bytes, size);
      }
      else {
        m_buffer = bytes;
        bytes = nullptr;
      }
      m_current = m_buffer;
      m_size = size;
      m_memsize = size;
      m_mode = std::ios_base::in | std::ios_base::binary;
    }
    
    /// Destructor. Always call delete on char buffer
    ~buffer() {
      if(nullptr != m_buffer) {
        delete [] m_buffer;
      }
      m_buffer = nullptr;
      m_current = nullptr;
    }
    
    /// Copy the input buffer. Returns the number of copied len
    /// Reset the position of the buffer at beginning
    size_type bufcpy(char_type *data, size_type size) {
      if(nullptr == data) {
        setstate(std::ios_base::badbit);
        return 0;
      }
      // save a memory allocation if buffer size if
      // buffer size in memory is bigger
      if(size > m_memsize) {
        if(nullptr != m_buffer) {
          delete [] m_buffer;
          m_buffer = nullptr;
        }
        allocator_type allocator;
        m_buffer = allocator.allocate(size);
      }
      std::memcpy(m_buffer, data, size);
      m_size = size;
      setstate(std::ios_base::goodbit);
      m_current = m_buffer;
      return m_size;
    }
    
    /// Seek the pointer in the buffer by an offset in the specifed way
    pos_type seekoff(off_type off, seek_dir way) {
      // from beginning
      if(std::ios_base::beg == way) {
        if(off > m_size) {
          setstate(std::ios_base::failbit);
        }
        else {
          m_current = begin() + off;
        }
      }
      // from end of buffer
      else if(std::ios_base::end == way) {
        if(off > m_size) {
          setstate(std::ios_base::failbit);
        }
        else {
          m_current = end() - off;      
        }
      }
      // from current position
      else {
        if(off > (end() - current())) {
          setstate(std::ios_base::failbit);
        }
        else {
          m_current = current() + off;      
        }
      }
      return tell();
    }
    
    /// Seek the position to an absolute value
    pos_type seekpos(pos_type pos) {
      return seekoff(pos, std::ios_base::beg);
    }
    
    /// Get the distance from beginning of buffer
    pos_type tell() const {
      return static_cast<pos_type>(begin() + current());
    }
    
    /// Expand the buffer by adding 'len' char at the end of buffer
    /// Returns the number of char added
    size_type expand(size_type len) {
      
    }
    
    /// Read a bunch of data. 
    /// Returns the size of actual read data
    /// Returns 0 if not in read mode 
    size_type read(char_type *data, size_type len) {
      
    }
    
    /// Write a bunch of data. 
    /// Returns the size of actual written data
    /// Returns 0 if not in write mode 
    size_type write(char_type *data, size_type len) {
      
    }
    
    /// Get the opening mode
    inline open_mode mode() const noexcept {
      return m_mode;
    }
    
    /// Get position at the begining of the buffer
    inline const char_type* begin() const noexcept {
      return m_buffer; 
    }
    
    /// Get the current ptr position 
    inline const char_type* current() const noexcept {
      return m_current;
    }
    
    /// Get the pointer at end of the buffer
    /// WARNING: this pointer can be changed in write
    /// if the buffer is expanded
    inline const char_type* end() const noexcept {
      return ((nullptr == m_buffer) ? nullptr : (m_buffer + m_size)); 
    }
    
    /// non-const version of begin
    inline char_type* begin() noexcept {
      return m_buffer;
    }
    
    /// non-const version of current
    inline char_type* current() noexcept {
      return m_current;
    }
    
    /// non-const version of end
    inline char_type* end() noexcept {
      return ((nullptr == m_buffer) ? nullptr : (m_buffer + m_size));
    }
    
    /// Returns the buffer state
    inline io_state rdstate() const noexcept {
      return m_iostate;
    }
    
    /// Set the buffer state 
    inline void setstate(io_state state) {
      m_iostate = state;
    }
    
    /// Whether the buffer is in a good state
    inline bool good() const noexcept {
      return (m_iostate & std::ios_base::goodbit;
    }
    
    inline bool eof() const noexcept {
      
    }
    
    inline bool fail() const noexcept {
      
    }
    
    inline bool bad() const noexcept {
      
    }
    
  private:
    /// The buffer open mode
    open_mode                  m_mode{0};
    /// The buffer io state
    io_state                   m_iostate{std::ios_base::goodbit};
    /// The buffer size
    size_type                  m_size{0};
    /// The buffer size in memory
    size_type                  m_memsize{0};
    /// The actual raw buffer
    char_type*                 m_buffer{nullptr};
    /// The current read/write position in the buffer
    char_type*                 m_current{nullptr};
  };
  
}
 
#endif
