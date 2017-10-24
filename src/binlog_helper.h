#pragma once

#include <cstdarg>
#include <math.h>

// define unpacking macros

/** Unpack a "reverse" byte order value */
#define unpack4(data) (data[3] + (data[2] << 8) + (data[1] << 16) + (data[0] << 24))
#define unpack3(data) (data[2] + (data[1] << 8) + (data[0] << 16))
/**
 * Unpack a 5 byte reverse byte order value
 * @param data pointer to data
 * @return Unpacked value
 */
static inline uint64_t unpack5(uint8_t* data)
{
    uint64_t rval = data[4];
    rval += ((uint64_t)data[3]) << 8;
    rval += ((uint64_t)data[2]) << 16;
    rval += ((uint64_t)data[1]) << 24;
    rval += ((uint64_t)data[0]) << 32;
    return rval;
}

/** Base-10 logarithm values */
const int64_t log_10_values[] =
{
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000
};

/**
 * If the TABLE_COL_TYPE_DATETIME type field is declared as a datetime with
 * extra precision, the packed length is shorter than 8 bytes.
 */
const size_t datetime_sizes[] =
{
    5, // DATETIME(0)
    6, // DATETIME(1)
    6, // DATETIME(2)
    7, // DATETIME(3)
    7, // DATETIME(4)
    7, // DATETIME(5)
    8  // DATETIME(6)
};

#define MAX_DATE_STRING_REP_LENGTH 30

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

template<typename T, typename U = unsigned char *>
inline void mem_alloc_cpy_str(T *dst, size_t size_alloc, U *src)
{
  mem_alloc_cpy<T, U>(dst, size_alloc+1, src, size_alloc);
  (*dst)[size_alloc] = '\0';
}


bool fixed_string_is_enum(drizzle_column_type_t type);
bool column_is_fixed_string(drizzle_column_type_t type);

uint get_metadata_len(drizzle_column_type_t column_type);

size_t unpack_enum(uint8_t *ptr, uint8_t *metadata, uint8_t *dest);

size_t unpack_numeric_field(uint8_t *src, uint8_t type, uint8_t *dest);

size_t unpack_decimal_field_length(uint precision, uint decimals);

size_t unpack_decimal_field ( unsigned char *ptr, uint precision, uint decimals,
    double *value);

uint64_t unpack_bytes(uint8_t *ptr, size_t bytes);

size_t unpack_temporal_value(drizzle_column_type_t column_type, unsigned char *ptr,
    unsigned char *metadata, int length, struct tm *tm);

size_t temporal_field_size(uint8_t type, uint8_t decimals);

int lookup_field_bytesize(drizzle_column_type_t field_type);

drizzle_field_datatype_t get_field_datatype(drizzle_column_type_t field_type);

