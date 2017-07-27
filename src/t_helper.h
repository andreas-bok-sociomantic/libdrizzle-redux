#pragma once

#include <limits>
#include <cstdint>
#include <type_traits>

template <typename U, int V = sizeof(U)> U
drizzle_binlog_read_bytes(int pos, const unsigned char* data);
//

#define bytes_col_count(__b) \
    ((uint64_t)(__b)<0xfb ? 1 : \
     ((uint64_t)(__b)==0xfc ? 2 : \
      ((uint64_t)(__b)==0xfd ? 3 : 8)))

#define mask(__b) \
    ((uint32_t)(__b)==32 ? 0xffffffff : \
     ((uint32_t)(__b)==24 ? 0xffffff : \
      ((uint32_t)(__b)==16 ? 0xffff : \
       ((uint32_t)(__b)==8 ? 0xff : 0xffffffffffffffff ))))
