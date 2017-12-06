#pragma once

#include <cstdarg>
#include <math.h>

// define unpacking macros

/** Unpack a "reverse" byte order value
 *
 * @param      data  input
 *
 * @return     data in reverse byte order
 */
#define unpack3(data) (data[2] + (data[1] << 8) + (data[0] << 16))

/** Unpack a "reverse" byte order value
 *
 * @param      data  input
 *
 * @return     data in reverse byte order
 */
#define unpack4(data) (data[3] + (data[2] << 8) + (data[1] << 16) + (data[0] << 24))

/**
 * Unpack a 5 byte reverse byte order value
 *
 * @param      data  pointer to data
 *
 * @return     Unpacked value
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
 * @param      dst   The destination struct
 * @param      src   The source struct
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

/**
 * @brief      Allocate memory and copy a char array. Adds a terminating '\0' to
 *             the allocated char array
 *
 * @param      dst         The destination
 * @param[in]  size_alloc  The size in bytes to allocate
 * @param      src         The source
 *
 * @tparam     T           destination pointer type
 * @tparam     U           source pointer type
 */
template<typename T, typename U = unsigned char *>
inline void mem_alloc_cpy_str(T *dst, size_t size_alloc, U *src)
{
  mem_alloc_cpy<T, U>(dst, size_alloc+1, src, size_alloc);
  (*dst)[size_alloc] = '\0';
}

/**
 * Check if a column is an ENUM or SET
 *
 * @param      type  Column type
 *
 * @return     True if column is either ENUM or SET
 */
bool fixed_string_is_enum(drizzle_column_type_t type);

/**
 * @brief      Check if the column is a string type column
 *
 * @param      type  Type of the column
 *
 * @return     True if the column is a string type column
 * @see        mxs_lestr_consume
 */
bool column_is_fixed_string(drizzle_column_type_t type);

/**
 * @brief      Gets the metadata length.
 *
 * @param[in]  column_type  The column type
 *
 * @return     The metadata length.
 */
uint get_metadata_len(drizzle_column_type_t column_type);

/**
 * @brief      Unpack an ENUM or SET field
 *
 * @param      ptr       Pointer to packed value
 * @param      metadata  Pointer to field metadata
 * @param      dest      The destination
 *
 * @return     Length of the processed field in bytes
 */
size_t unpack_enum(uint8_t *ptr, uint8_t *metadata, uint8_t *dest);

/**
 * @brief      Unpack a numeric field value
 *
 * @param      src   Pointer to the packed value
 * @param[in]  type  The column type
 * @param      dest  Pointer to the destination
 *
 * @return     The size in bytes of the unpacked type
 */
size_t unpack_numeric_field(uint8_t *src, uint8_t type, uint8_t *dest);

/**
 * @brief      Unpack the size of the decimal in bytes
 *
 * @param[in]  precision  The precision
 * @param[in]  decimals   The number of decimals
 *
 * @return     The size of the decimal in bytes
 */
size_t unpack_decimal_field_length(uint precision, uint decimals);

/**
 * @brief      Unpack a decimal from a raw bytes and assign the value to a
 *             double variable
 *
 * @param      ptr        Pointer to packed value
 * @param[in]  precision  The precision
 * @param[in]  decimals   The number of decimals
 * @param      value      Pointer to target value
 *
 * @return     The size of the decimal in bytes
 */
size_t unpack_decimal_field ( unsigned char *ptr, uint precision, uint decimals,
    double *value);

/**
 * @brief      Unpack bytes
 *
 * @param      ptr    The pointer to the input
 * @param[in]  bytes  The number of bytes to unpack
 *
 * @return     The unpacked value as unsigned 8-byte integer
 */
uint64_t unpack_bytes(uint8_t *ptr, size_t bytes);

/**
 * @brief      Unpack a temporal value
 *
 *             MariaDB and MySQL both store temporal values in a special format.
 *             This function unpacks them from the storage format and into a
 *             common, usable format.
 *
 * @param[in]  column_type  The column type
 * @param      ptr          The pointer
 * @param      metadata     The metadata
 * @param[in]  length       The length
 * @param      tm           Pointer where the unpacked temporal value is stored
 *
 * @return     The size of the unpacked temporal value in bytes
 */
size_t unpack_temporal_value(drizzle_column_type_t column_type, unsigned char *ptr,
    unsigned char *metadata, int length, struct tm *tm);

/**
 * @brief      Get the size of a temporal type in bytes
 *
 * @param[in]  type      The type
 * @param[in]  decimals  The number of decimals
 *
 * @return     The size of a temporal type in bytes
 */
size_t temporal_field_size(uint8_t type, uint8_t decimals);

/**
 * @brief      Get the size of a drizzle column type in bytes
 *             For length encoded column types the returned value is -1
 *
 * @param[in]  field_type  The column type of the field
 *
 * @return     The size of a drizzle column type in bytes
 */
int lookup_field_bytesize(drizzle_column_type_t field_type);

/**
 * @brief      Gets the field datatype.
 *
 * @param[in]  field_type  The field type
 *
 * @return     The field datatype.
 */
drizzle_field_datatype_t get_field_datatype(drizzle_column_type_t field_type);

/**
 * @brief      { function_description }
 *
 * @param      rbr       The rbr
 * @param[in]  table_id  The table identifier
 *
 * @return     { description_of_the_return_value }
 */
const char *schema_name_from_tableid(drizzle_binlog_rbr_st *rbr, size_t table_id);
