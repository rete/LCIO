

#include <fio/stream.h>

namespace fio {

  error_codes::code_type stream::read_next_record_info(record_info &info) {
    if((m_openmode != io::open_mode::read) and m_openmode != io::open_mode::read_write) {
      return error_codes::stream::bad_mode;
    }
    if((m_openstate != io::open_state::opened)) {
      return error_codes::stream::not_open;
    }

    return error_codes::stream::success;
  }

}
