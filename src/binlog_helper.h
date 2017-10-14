#pragma once

#include <cstdarg>

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
bool fixed_string_is_enum(drizzle_column_type_t type);
bool column_is_fixed_string(drizzle_column_type_t type);


uint get_metadata_len(drizzle_column_type_t column_type);

size_t unpack_enum(uint8_t *ptr, uint8_t *metadata, uint8_t *dest);

size_t unpack_numeric_field(uint8_t *src, uint8_t type, uint8_t *dest);
