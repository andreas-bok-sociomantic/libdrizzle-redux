#pragma once

#include <cstdarg>
/**
 * @brief      Get the version of the RBR replication Rows Event
 *
 * @param[in]  column_type  a column type
 *
 * @return     Return either 1 or 2 or 0 if column type wasn't found
 */
uint8_t drizzle_binlog_rows_event_version(
    drizzle_binlog_event_types_t column_type);

/**
 * @brief      Get the distance between two pointers in a char array
 *
 * @param      ptr1  A pointer in the a char array
 * @param      ptr2  A pointer in the a char array
 *
 * @return     distance in bytes between pointers
 */
uint64_t ptr_dist(unsigned char *ptr1, unsigned char *ptr2);


/**
 * @brief      Get remaining bytes in the binlog event
 *
 * @param      event  a drizzle_binlog_event struct
 *
 * @return     number of available bytes
 */
uint64_t drizzle_binlog_event_available_bytes(drizzle_binlog_event_st *event);

/**
 * @brief      Gets the bytes for a length-encoded numeric value from raw bytes
 *
 * @param      binlog_event  A pointer to a binlog event struct
 *
 * @return     the encoded length
 */
uint64_t drizzle_binlog_get_encoded_len(drizzle_binlog_event_st *binlog_event);


/**
 * Copy member values from one header to another.
 *
 * Raw event data is not copied
 *
 * @param      dst    The destination struct
 * @param      src    The source struct
 */
void set_event_header(drizzle_binlog_event_st* dst,
    drizzle_binlog_event_st* src);

/*#define get_var_arg(arg, last_arg, ...)
{
    va_list args;
    va_start(args, last_arg);
    auto i = 0;
    while (i < n && (*arg = va_arg(args, U)) != NULL)
    {
        i++;
    }
    va_end(*args);
    return (i == n || arg != NULL);
}

*//**
 * Check if a bit is set
 *
 * @param ptr Pointer to start of bitfield
 * @param columns Number of columns (bits)
 * @param current_column Zero indexed column number
 * @return True if the bit is set
 */
//bool bit_is_set(uint8_t *ptr, int columns, int current_column);


template <typename T>
bool __get_var_arg(uint n, T *val, ...) {
  uint arg_idx = 0;
  va_list args;
  va_start(args, val);
  bool ret_val = false;
  T arg;
  while (true) {
    arg = va_arg(args, T);
    if (arg == NULL || arg_idx > n)
    {
      break;
    }
    else if (arg_idx == n) {
        *val= arg;
        ret_val = true;
    }
    arg_idx++;
  }
  va_end(args);
  return ret_val;
}

#define var_arg_const_char(...) \
    __get_var_arg<const char*>(__VA_ARGS__, NULL)

#define var_arg_uint(...) \
    __get_var_arg<uint32_t>(__VA_ARGS__, NULL)

#define var_arg_char(...) \
    __get_var_arg<char*>(__VA_ARGS__, NULL)

/**
 * @brief      Allocate memory and copy from a source to a destination pointer
 *
 * @param      dst         The destination pointer
 * @param[in]  size_alloc  The number of bytes to allocate
 * @param      src         The source
 * @param[in]  size_copy   The number of bytes to copy
 *
 * @tparam     T           destination pointer type
 * @tparam     U           source pointer type
 */
template<typename T, typename U = unsigned char *>
inline void mem_alloc_cpy(T *dst, size_t size_alloc, U *src, size_t size_copy)
{
  *dst = dst == NULL ? (T) malloc(size_alloc) : (T) realloc(*dst, size_alloc);
  if (src != NULL)
  {
    memcpy(*dst, *src, size_copy);
  }
}

/**
 * @brief      Allocate memory and copy from a source to a destination pointer
 *
 * @param      dst         The destination
 * @param[in]  size_alloc  The size in bytes to allocate
 * @param      src         The source
 *
 * @tparam     T           destination pointer type
 * @tparam     U           source pointer type
 */
template<typename T, typename U = unsigned char *>
inline void mem_alloc_cpy(T *dst, size_t size_alloc, U *src=NULL)
{

  mem_alloc_cpy<T, U>(dst, size_alloc, src, size_alloc);
}

