
#ifndef FIO_STREAM_H
#define FIO_STREAM_H 1

// -- fio headers
#include <fio/definitions.h>

// -- std headers
#include <stdio.h>

#ifdef FIO_USE_DCAP
// -- dcap headers
#include <dcap.h>
#else
// -- std headers
#include <sys/stat.h>
#endif

namespace fio {

  /// stream class.
  ///
  /// Main interface to file stream (read or write)
  class stream {
  public:
    // I/O functions
#ifdef FIO_USE_DCAP
    using file_open =   dc_fopen;
    using file_tell =   dc_ftell;
    using file_seek =   dc_fseek;
    using file_close =  dc_fclose;
    using file_read =   dc_fread;
    using file_write =  dc_fwrite;
    using file_flush =  dc_fflush;
    using file_stat =   dc_stat;
#else
    using file_open =   fopen;
    using file_tell =   ftell;
    using file_seek =   fseek;
    using file_close =  fclose;
    using file_read =   fread;
    using file_write =  fwrite;
    using file_flush =  fflush;
    using file_stat =   stat;
#endif
    typedef buffer<unsigned char, big_endian_copy>   stream_buffer;

  public:
    stream() = delete;
    ~stream() = default;

    /// Constructor with stream name
    inline stream(const std::string &inname) :
      m_name(inname) {
      /* nop */
    }

    /// Get the name of stream
    inline const std::string& name() const noexcept {
      return m_name;
    }

    /// Get the file name
    inline const std::string& fname() const noexcept {
      return m_fname;
    }

    /// Get the open state (opened, closed, error)
    inline io::open_state open_state() const noexcept {
      return m_openstate;
    }

    /// Get the open mode
    inline io::open_mode open_mode() const noexcept {
      return m_openmode;
    }

    /// open a file
    error_code::code_type open(const std::string& fn, io::open_mode mode) noexcept;

    /// close the file
    error_code::code_type close() noexcept;

  private:
    /// The stream open mode
    io::open_mode              m_openmode{io::open_mode::read};
    /// The stream name
    std::string                m_name{};
    /// The current opened file name
    std::string                m_fname{};
    /// The stream open state
    io::open_state             m_openstate{io::open_state::closed};
  };
}

#endif
