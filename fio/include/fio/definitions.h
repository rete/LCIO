
#ifndef FIO_DEFINITIONS_H
#define FIO_DEFINITIONS_H 1

#include <map> // map, multimap

namespace fio {

  struct types {
  public:
#if defined(__alpha__) || defined(_M_ALPHA) || defined(_LP64)
    typedef std::size_t                   address;
#else
    typedef unsigned int                  address;
#endif
    typedef void                          ptr;
    typedef std::map<ptr*, ptr*>          pointed_at;
    typedef std::multimap<ptr*, ptr*>     pointer_to;
    // re: keep this for backward compatibility with SIO
#if defined(_AIX) ||  defined(__alpha__) || defined(__i386__) || defined(__sparc__) || defined(__APPLE_CC__) || defined(_LP64)
    typedef long long                     int64;
#else
    typedef __int64                       int64;
#endif

  public:
    /// Cast any pointer to unsigned char
    template <typename T>
    unsigned char* ptr_cast(T *ptr) {
      return reinterpret_cast<unsigned char *>(ptr);
    }
  };

  /// Definitions of error codes
  struct error_codes {
    /// The error code type
    typedef unsigned int           code_type;
    /// Error codes for records
    struct record {
      static constexpr code_type facility         = 0x08010000;
      static constexpr code_type success          = 0x08010001;
      static constexpr code_type not_found        = 0x08010014;
      static constexpr code_type bad_argument     = 0x08010024;
      static constexpr code_type dup_connect      = 0x08010034;
      static constexpr code_type not_connected    = 0x08010044;
      static constexpr code_type no_block_marker  = 0x08010054;
    };
    /// Error codes for blocks
    struct block {
      static constexpr code_type facility         = 0x08020000;
      static constexpr code_type success          = 0x08020001;
      static constexpr code_type not_found        = 0x08020014;
      static constexpr code_type skip             = 0x08020024;
    };
    struct stream {
      static constexpr code_type facility         = 0x08000000;
      static constexpr code_type success          = 0x08000001;
      static constexpr code_type not_found        = 0x08000014;
      static constexpr code_type already_open     = 0x08000024;
      static constexpr code_type open_fail        = 0x08000034;
      static constexpr code_type bad_mode         = 0x08000044;
      static constexpr code_type not_open         = 0x08000054;
      static constexpr code_type go_to_eof        = 0x08000064;
      static constexpr code_type read_only        = 0x08000074;
      static constexpr code_type write_only       = 0x08000084;
      static constexpr code_type no_such_record   = 0x08000094;
      static constexpr code_type bad_write        = 0x080000a4;
      static constexpr code_type bad_state        = 0x080000b4;
      static constexpr code_type off_end          = 0x080000c4;
      static constexpr code_type no_alloc         = 0x080000d4;
      static constexpr code_type eof              = 0x080000e4;
      static constexpr code_type no_record_marker = 0x080000f4;
      static constexpr code_type bad_compress     = 0x08000104;
    };
  };

  /// Version manipulation helper
  struct version {
    typedef unsigned int          version_type;

    /// Encode major and minor version in a single version variable
    static inline version_type encode(version_type maj, version_type min) noexcept {
      return (((maj) << 16) + (min));
    }

    /// Decode the major version from the full version variable
    static inline version_type decode_major(version_type version) noexcept {
      return (((version) & 0xffff0000) >> 16);
    }

    /// Decode the minor version from the full version variable
    static inline version_type decode_minor(version_type version) noexcept {
      return ((version) & 0x0000ffff);
    }
  };

  struct io {
    enum class open_state {
      closed,
      opened,
      error
    };

    enum class open_mode {
      read,
      write_new,
      write_append,
      read_write
    };

    inline std::string open_mode_str(open_mode mode) noexcept {
      std::string modestr;
      switch(mode) {
        case open_mode::read:         modestr = "rb"; break;
        case open_mode::write_new:    modestr = "wb"; break;
        case open_mode::write_append: modestr = "ab"; break;
        case open_mode::read_write:   modestr = "r+"; break;
        default: break;
      }
      return modestr;
    }
  };
}

#endif
