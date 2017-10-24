#include "config.h"
#include "src/common.h"

#include <type_traits>

#include <iostream>


/**
 * @brief      Gets the length encoded integer in bytes
 *
 * @param      __b   a char value
 *
 * @return     { description_of_the_return_value }
 */
#define bytes_col_count(__b) \
    ((uint64_t) (__b) < 0xfb ? 1 : \
     ((uint64_t) (__b) == 0xfc ? 2 : \
      ((uint64_t) (__b) == 0xfd ? 3 : 8)))

/**
 * @brief      Create a bitmask with all bits set to 1
 *
 * @param      __b   length of bitmask in bits
 *
 * @return     bitmask with a all bits set to 1
 */
#define mask(__b) \
    ((uint32_t) (__b) == 32 ? 0xffffffff : \
     ((uint32_t) (__b) == 24 ? 0xffffff : \
      ((uint32_t) (__b) == 16 ? 0xffff : \
       ((uint32_t) (__b) == 8 ? 0xff : 0xffffffffffffffff ))))


int lookup_field_bytesize(drizzle_column_type_t field_type)
{
    switch ( field_type )
    {
        case DRIZZLE_COLUMN_TYPE_DECIMAL:
        case DRIZZLE_COLUMN_TYPE_VARCHAR:
        case DRIZZLE_COLUMN_TYPE_BIT:
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
        case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
        case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
        case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
        case DRIZZLE_COLUMN_TYPE_BLOB:
        case DRIZZLE_COLUMN_TYPE_GEOMETRY:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
        case DRIZZLE_COLUMN_TYPE_STRING:
            return -1;

        case DRIZZLE_COLUMN_TYPE_TINY:
        case DRIZZLE_COLUMN_TYPE_YEAR:
            return 1;

        case DRIZZLE_COLUMN_TYPE_SHORT:
            return 2;

        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_TIME2:
        case DRIZZLE_COLUMN_TYPE_INT24:
        case DRIZZLE_COLUMN_TYPE_NEWDATE:
            return 3;

        case DRIZZLE_COLUMN_TYPE_LONG:
        case DRIZZLE_COLUMN_TYPE_FLOAT:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
        case DRIZZLE_COLUMN_TYPE_DATETIME:
            return 4;

        case DRIZZLE_COLUMN_TYPE_DATETIME2:
            return 5;

        case DRIZZLE_COLUMN_TYPE_DOUBLE:
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
            return 8;

        case DRIZZLE_COLUMN_TYPE_NULL:
        case DRIZZLE_COLUMN_TYPE_ENUM:
        case DRIZZLE_COLUMN_TYPE_SET:
        default:
            return 0;
    } // switch
} // lookupFieldByteSize

uint64_t unpack_bytes(uint8_t *ptr, size_t bytes)
{
    uint64_t val = 0;

    switch (bytes)
    {
        case 1:
            val = ptr[0];
            break;
        case 2:
            val = ptr[1] | ((uint64_t) (ptr[0]) << 8);
            break;
        case 3:
            val = (uint64_t) ptr[2] | ((uint64_t) ptr[1] << 8) |
                ((uint64_t) ptr[0] << 16);
            break;
        case 4:
            val = (uint64_t) ptr[3] | ((uint64_t) ptr[2] << 8) |
                ((uint64_t) ptr[1] << 16) | ((uint64_t) ptr[0] << 24);
            break;
        case 5:
            val = (uint64_t) ptr[4] | ((uint64_t) ptr[3] << 8) |
                ((uint64_t) ptr[2] << 16) | ((uint64_t) ptr[1] << 24) |
                ((uint64_t) ptr[0] << 32);
            break;
        case 6:
            val = (uint64_t) ptr[5] | ((uint64_t) ptr[4] << 8) |
                ((uint64_t) ptr[3] << 16) | ((uint64_t) ptr[2] << 24) |
                ((uint64_t) ptr[1] << 32) | ((uint64_t) ptr[0] << 40);
            break;
        case 7:
            val = (uint64_t) ptr[6] | ((uint64_t) ptr[5] << 8) |
                ((uint64_t) ptr[4] << 16) | ((uint64_t) ptr[3] << 24) |
                ((uint64_t) ptr[2] << 32) | ((uint64_t) ptr[1] << 40) |
                ((uint64_t) ptr[0] << 48);
            break;
        case 8:
            val = (uint64_t) ptr[7] | ((uint64_t) ptr[6] << 8) |
                ((uint64_t) ptr[5] << 16) | ((uint64_t) ptr[4] << 24) |
                ((uint64_t) ptr[3] << 32) | ((uint64_t) ptr[2] << 40) |
                ((uint64_t) ptr[1] << 48) | ((uint64_t) ptr[0] << 56);
            break;

        default:
            assert(false);
            break;
    } // switch

    return val;
} // unpack_bytes


uint8_t drizzle_binlog_rows_event_version(
    drizzle_binlog_event_types_t event_type)
{
    return (event_type == DRIZZLE_EVENT_TYPE_V1_WRITE_ROWS ||
            event_type == DRIZZLE_EVENT_TYPE_V1_UPDATE_ROWS ||
            event_type == DRIZZLE_EVENT_TYPE_V1_DELETE_ROWS) ? 1 :
           (event_type == DRIZZLE_EVENT_TYPE_V2_WRITE_ROWS ||
            event_type == DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS ||
            event_type == DRIZZLE_EVENT_TYPE_V2_DELETE_ROWS) ? 2 : 0;
}

void dump_array_to_hex(const unsigned char *ptr, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        std::cout << std::hex << (uint32_t) ptr[i] << " ";
        if (i > 0 && i % 7 == 0 )
        {
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

uint64_t ptr_dist(unsigned char *ptr1, unsigned char *ptr2)
{
    return ((const char *) ptr1 - (const char *) ptr2);
}

uint64_t drizzle_binlog_event_available_bytes(drizzle_binlog_event_st *event)
{
    return (event->length - ptr_dist(event->data_ptr, event->data));
}

uint64_t drizzle_binlog_get_encoded_len(drizzle_binlog_event_st *binlog_event)
{

    uint64_t len = 0;

    if (drizzle_binlog_event_available_bytes(binlog_event) < 1)
    {
        return 0;
    }

    uint32_t colBytes = bytes_col_count((uint32_t) binlog_event->data_ptr[0]);
    if (drizzle_binlog_event_available_bytes(binlog_event) - 1 < colBytes)
    {
        binlog_event->data_ptr++;
        return 0;
    }
    switch (colBytes)
    {
        case 1:
            len = binlog_event->data_ptr[0];
            binlog_event->data_ptr++;
            break;
        case 2:
            binlog_event->data_ptr++;
            len = drizzle_get_byte2(binlog_event->data_ptr);
            break;
        case 3:
            binlog_event->data_ptr++;
            len = drizzle_get_byte3(binlog_event->data_ptr);
            break;
        case 8:
            binlog_event->data_ptr++;
            len = drizzle_get_byte3(binlog_event->data_ptr);
            break;
        default:
            break;
    }

    binlog_event->data_ptr += colBytes > 1 ? colBytes : 0;

    return len;
} // drizzle_binlog_get_encoded_len


void set_event_header(drizzle_binlog_event_st *dst,
                      drizzle_binlog_event_st *src)
{
    dst->timestamp = src->timestamp;
    dst->type = src->type;
    dst->server_id = src->server_id;
    dst->length = src->length;
    dst->next_pos = src->next_pos;
    dst->flags = src->flags;
    dst->extra_flags = src->extra_flags;
    dst->checksum = src->checksum;
    dst->raw_length = src->raw_length;
}


bool drizzle_binlog_is_rows_event(const drizzle_binlog_event_types_t event_type)
{
    return (event_type == DRIZZLE_EVENT_TYPE_V1_WRITE_ROWS ||
            event_type == DRIZZLE_EVENT_TYPE_V1_UPDATE_ROWS ||
            event_type == DRIZZLE_EVENT_TYPE_V1_DELETE_ROWS ||
            event_type == DRIZZLE_EVENT_TYPE_V2_WRITE_ROWS ||
            event_type == DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS ||
            event_type == DRIZZLE_EVENT_TYPE_V2_DELETE_ROWS );
}


/**
 * @brief      Check if a bit is set
 *
 * @param      ptr             Pointer to start of bitfield
 * @param      current_column  Zero indexed column number
 * @param      columns  Number of columns (bits)
 *
 * @return     True if the bit is set
 */
bool bit_is_set(const unsigned char *ptr, int current_column)
{
    if (current_column >= 8)
    {
        ptr += current_column / 8;
        current_column = current_column % 8;
    }

    return ((*ptr) & (1 << current_column));
}

/**
 * Check if a column is an ENUM or SET
 *
 * @param      type  Column type
 *
 * @return     True if column is either ENUM or SET
 */
bool fixed_string_is_enum(drizzle_column_type_t type)
{
    return type == DRIZZLE_COLUMN_TYPE_ENUM || type == DRIZZLE_COLUMN_TYPE_SET;
}

/**
 * @brief      Check if the column is a string type column
 *
 * @param      type  Type of the column
 *
 * @return     True if the column is a string type column
 * @see        mxs_lestr_consume
 */
bool column_is_fixed_string(drizzle_column_type_t type)
{
    return type == DRIZZLE_COLUMN_TYPE_VAR_STRING;
}


/**
 * @brief      Unpack an ENUM or SET field
 *
 * @param      ptr       Pointer to packed value
 * @param      metadata  Pointer to field metadata
 * @param      dest      The destination
 *
 * @return     Length of the processed field in bytes
 */
size_t unpack_enum(uint8_t *ptr, uint8_t *metadata, uint8_t *dest)
{
    memcpy(dest, ptr, metadata[1]);
    return metadata[1];
}


drizzle_column_protocol_datatype_t column_protocol_datatype(
    drizzle_column_type_t type)
{
    switch ( type )
    {
        case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
        case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
        case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
        case DRIZZLE_COLUMN_TYPE_BLOB:
            return BLOB;

        case DRIZZLE_COLUMN_TYPE_DECIMAL:
        case DRIZZLE_COLUMN_TYPE_VARCHAR:
        case DRIZZLE_COLUMN_TYPE_BIT:
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
        case DRIZZLE_COLUMN_TYPE_GEOMETRY:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
            return VARIABLE_STRING;

        case DRIZZLE_COLUMN_TYPE_YEAR:
        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_TIME2:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
        case DRIZZLE_COLUMN_TYPE_DATETIME:
        case DRIZZLE_COLUMN_TYPE_DATETIME2:
            return TEMPORAL;

        // DRIZZLE_COLUMN_TYPE_ENUM and DRIZZLE_COLUMN_TYPE_SET are
        // packed as a DRIZZLE_COLUMN_TYPE_STRING and cannot appear as a
        // type in the fixed header. Their respective types must be
        // extracted from the column metadata
        case DRIZZLE_COLUMN_TYPE_STRING:
            return FIXED_STRING;

        case DRIZZLE_COLUMN_TYPE_TINY:
        case DRIZZLE_COLUMN_TYPE_SHORT:
        case DRIZZLE_COLUMN_TYPE_INT24:
        case DRIZZLE_COLUMN_TYPE_LONG:
        case DRIZZLE_COLUMN_TYPE_FLOAT:
        case DRIZZLE_COLUMN_TYPE_DOUBLE:
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
        case DRIZZLE_COLUMN_TYPE_NULL:
            return NUMERICAL;

        case DRIZZLE_COLUMN_TYPE_ENUM:
        case DRIZZLE_COLUMN_TYPE_SET:
        case DRIZZLE_COLUMN_TYPE_NEWDATE:
        default:
            return NOT_FOUND;
    } // switch
} // column_protocol_datatype



uint get_metadata_len(drizzle_column_type_t column_type)
{
    switch ( column_type )
    {
        // Note that the following column types were added in v5.0
        // - DRIZZLE_COLUMN_TYPE_DATETIME2
        // - DRIZZLE_COLUMN_TYPE_TIME2
        // - DRIZZLE_COLUMN_TYPE_TIMESTAMP2
        //
        // The column types and their metadata byte size is however not
        // covered in the accessible MySQL source code documentation,
        // (https://is.gd/IPzVrf), cf.
        // "Table_map_event column types: numerical identifier and metadata"
        //
        // Instead the metadata byte size of 1 is based on how MaxScale
        // handles these column types, cf. (avro_rbr.c)
        case DRIZZLE_COLUMN_TYPE_FLOAT:
        case DRIZZLE_COLUMN_TYPE_BLOB:
        case DRIZZLE_COLUMN_TYPE_GEOMETRY:
        case DRIZZLE_COLUMN_TYPE_DOUBLE:
        case DRIZZLE_COLUMN_TYPE_TIME2:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
        case DRIZZLE_COLUMN_TYPE_DATETIME2:
            return 1;

        case DRIZZLE_COLUMN_TYPE_BIT:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
        case DRIZZLE_COLUMN_TYPE_VARCHAR:
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
        case DRIZZLE_COLUMN_TYPE_STRING:
            return 2;

        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_DATETIME:
        case DRIZZLE_COLUMN_TYPE_DECIMAL: // Pre 5.0
        case DRIZZLE_COLUMN_TYPE_INT24:
        case DRIZZLE_COLUMN_TYPE_LONG:
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
        case DRIZZLE_COLUMN_TYPE_NULL:
        case DRIZZLE_COLUMN_TYPE_SHORT:
        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_TINY:
        case DRIZZLE_COLUMN_TYPE_YEAR:
        // Not used but handled due to -Werror=switch-enum compiler switch
        case DRIZZLE_COLUMN_TYPE_NEWDATE:
        case DRIZZLE_COLUMN_TYPE_ENUM:
        case DRIZZLE_COLUMN_TYPE_SET:
        case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
        case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
        case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
        default:
            return 0;
    } // switch
} // get_metadata_len

size_t unpack_numeric_field(uint8_t *src, uint8_t type, uint8_t *dest)
{
    size_t size = 0;

    switch (type)
    {
        case DRIZZLE_COLUMN_TYPE_LONG:
        case DRIZZLE_COLUMN_TYPE_FLOAT:
            size = 4;
            break;

        case DRIZZLE_COLUMN_TYPE_INT24:
            size = 3;
            break;

        case DRIZZLE_COLUMN_TYPE_LONGLONG:
        case DRIZZLE_COLUMN_TYPE_DOUBLE:
            size = 8;
            break;

        case DRIZZLE_COLUMN_TYPE_SHORT:
            size = 2;
            break;

        case DRIZZLE_COLUMN_TYPE_TINY:
            size = 1;
            break;

        default:
            // MXS_ERROR("Bad column type: %x %s", type,
            // column_type_to_string(type));
            break;
    } // switch

    memcpy(dest, src, size);
    return size;
} // unpack_numeric_field

bool is_rows_update_event(drizzle_binlog_rows_event_st *rows_event)
{
    return rows_event->header.type == DRIZZLE_EVENT_TYPE_V1_UPDATE_ROWS ||
           rows_event->header.type == DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS;
}

size_t unpack_decimal_field_length(uint precision, uint decimals)
{
    const int dec_dig = 9;
    const int dig_bytes[] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 4 };

    uint ipart = precision - decimals;
    uint ipart1 = ipart / dec_dig;
    uint fpart1 = decimals / dec_dig;
    uint ipart2 = ipart - ipart1 * dec_dig;
    uint fpart2 = decimals - fpart1 * dec_dig;
    uint ibytes = ipart1 * 4 + dig_bytes[ipart2];
    uint fbytes = fpart1 * 4 + dig_bytes[fpart2];

    return ibytes + fbytes;
}

size_t unpack_decimal_field(unsigned char *ptr, uint precision, uint decimals,
                          double *value)
{
    const int dec_dig = 9;
    const int dig_bytes[] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 4 };

    uint ipart = precision - decimals;
    uint ipart1 = ipart / dec_dig;
    uint fpart1 = decimals / dec_dig;
    uint ipart2 = ipart - ipart1 * dec_dig;
    uint fpart2 = decimals - fpart1 * dec_dig;
    uint ibytes = ipart1 * 4 + dig_bytes[ipart2];
    uint fbytes = fpart1 * 4 + dig_bytes[fpart2];

    bool negative = (ptr[0] & 0x80) == 0;

    ptr[0] ^= 0x80;

//    auto decimal_bytes = ibytes + fbytes;

    if ( negative )
    {
        // Do a bitwise NOT on the integer and fractional parts of the value
        for (uint i = 0; i < ibytes; i++ )
        {
            ptr[i] = ~ptr[i];
        }


        for (uint i = 0; i < fbytes; i++)
        {
            ptr[i + ibytes] = ~ptr[i + ibytes];
        }
    }

    int64_t val_i = unpack_bytes(ptr, ibytes);
    int64_t val_f = fbytes ? unpack_bytes(ptr + ibytes, fbytes) : 0;

    if (negative)
    {
        val_i = -val_i;
        val_f = -val_f;
    }

    *value = (double) val_i + ((double) val_f / (pow(10.0, decimals)));

    return ibytes + fbytes;
} // unpack_decimal_field

/**
 * @brief Unpack a YEAR type
 *
 * The value seems to be stored as an offset from the year 1900
 * @param val Stored value
 * @param dest Destination where unpacked value is stored
 */
static void unpack_year(uint8_t *ptr, struct tm *dest)
{
    memset(dest, 0, sizeof(*dest));
    dest->tm_year = *ptr;
}

/**
 * @brief Unpack a DATETIME
 *
 * The DATETIME is stored as a 8 byte value with the values stored as multiples
 * of 100. This means that the stored value is in the format YYYYMMDDHHMMSS.
 * @param val Value read from the binary log
 * @param dest Pointer where the unpacked value is stored
 */
static void unpack_datetime(uint8_t *ptr, int length, struct tm *dest)
{
    int64_t val = 0;
    uint32_t second, minute, hour, day, month, year;

    if (length == -1)
    {
        val = drizzle_get_byte8(ptr);
        second = val - ((val / 100) * 100);
        val /= 100;
        minute = val - ((val / 100) * 100);
        val /= 100;
        hour = val - ((val / 100) * 100);
        val /= 100;
        day = val - ((val / 100) * 100);
        val /= 100;
        month = val - ((val / 100) * 100);
        val /= 100;
        year = val;
    }
    else
    {
        // TODO: Figure out why DATETIME(0) doesn't work like it others do
        val = unpack_bytes(ptr, datetime_sizes[length]);
        val *= log_10_values[6 - length];

        if (val < 0)
        {
            val = -val;
        }


        // TODO this is unused but might be an error
        // int subsecond = val % 1000000;
        val /= 1000000;

        second = val % 60;
        val /= 60;
        minute = val % 60;
        val /= 60;
        hour = val % 24;
        val /= 24;
        day = val % 32;
        val /= 32;
        month = val % 13;
        val /= 13;
        year = val;
    }

    memset(dest, 0, sizeof(struct tm));
    dest->tm_year = year - 1900;
    dest->tm_mon = month - 1;
    dest->tm_mday = day;
    dest->tm_hour = hour;
    dest->tm_min = minute;
    dest->tm_sec = second;
} // unpack_datetime

/** The DATETIME values are stored in the binary logs with an offset */
// const unsigned long long DATETIME2_OFFSET=0x8000000000LL
#define DATETIME2_OFFSET 549755813888

/**
 * @brief Unpack a DATETIME2
 *
 * The DATETIME2 is only used by row based replication in newer MariaDB servers.
 * @param val Value read from the binary log
 * @param dest Pointer where the unpacked value is stored
 */
static void unpack_datetime2(uint8_t *ptr, struct tm *dest)
{
    int64_t unpacked = unpack5(ptr) - DATETIME2_OFFSET;

    if (unpacked < 0)
    {
        unpacked = -unpacked;
    }

    uint64_t date = unpacked >> 17;
    uint64_t yearmonth = date >> 5;
    uint64_t time = unpacked % (1 << 17);

    memset(dest, 0, sizeof(*dest));
    dest->tm_sec = time % (1 << 6);
    dest->tm_min = (time >> 6) % (1 << 6);
    dest->tm_hour = time >> 12;
    dest->tm_mday = date % (1 << 5);
    dest->tm_mon = (yearmonth % 13) - 1;

    /** struct tm stores the year as: Year - 1900 */
    dest->tm_year = (yearmonth / 13) - 1900;
} // unpack_datetime2

/**
 * @brief Unpack a TIMESTAMP
 *
 * The timestamps are stored with the high bytes first
 * @param val The stored value
 * @param dest Destination where the result is stored
 */
static void unpack_timestamp(uint8_t *ptr, struct tm *dest)
{
    time_t t = unpack4(ptr);

    localtime_r(&t, dest);
}

/**
 * @brief Unpack a TIME
 *
 * The TIME is stored as a 3 byte value with the values stored as multiples
 * of 100. This means that the stored value is in the format HHMMSS.
 * @param val Value read from the binary log
 * @param dest Pointer where the unpacked value is stored
 */
static void unpack_time(uint8_t *ptr, struct tm *dest)
{
    uint64_t val = drizzle_get_byte3(ptr);
    uint32_t second = val - ((val / 100) * 100);

    val /= 100;
    uint32_t minute = val - ((val / 100) * 100);
    val /= 100;
    uint32_t hour = val;

    memset(dest, 0, sizeof(struct tm));
    dest->tm_hour = hour;
    dest->tm_min = minute;
    dest->tm_sec = second;
}

/**
 * @brief Unpack a DATE value
 * @param ptr Pointer to packed value
 * @param dest Pointer where the unpacked value is stored
 */
static void unpack_date(uint8_t *ptr, struct tm *dest)
{
    uint64_t val = ptr[0] + (ptr[1] << 8) + (ptr[2] << 16);

    memset(dest, 0, sizeof(struct tm));
    dest->tm_mday = val & 31;
    dest->tm_mon = ((val >> 5) & 15) - 1;
    dest->tm_year = (val >> 9) - 1900;
}

/**
 * @brief Get the length of a temporal field
 * @param type Field type
 * @param decimals How many decimals the field has
 * @return Number of bytes the temporal value takes
 */


/* Original implementation from maxscale. It isn't quite clear though how the
 * length parameter is extracted
 *
 * static size_t temporal_field_size(uint8_t type, uint8_t decimals,
 *  int length)
 * {
 *  switch (type)
 *  {
 *  case DRIZZLE_COLUMN_TYPE_YEAR:
 *      return 1;
 *
 *  case DRIZZLE_COLUMN_TYPE_TIME:
 *  case DRIZZLE_COLUMN_TYPE_DATE:
 *      return 3;
 *
 *  case DRIZZLE_COLUMN_TYPE_TIME2:
 *      return 3 + ((decimals + 1) / 2);
 *
 *  case DRIZZLE_COLUMN_TYPE_DATETIME:
 *      return length < 0 || length > 6 ? 8 : datetime_sizes[length];
 *
 *  case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
 *      return 4;
 *
 *  case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
 *      return 4 + ((decimals + 1) / 2);
 *
 *  case DRIZZLE_COLUMN_TYPE_DATETIME2:
 *      return 5 + ((decimals + 1) / 2);
 *
 *  default:
 *      printf("Unknown field type: %d %s", type,
 * drizzle_column_type_str((drizzle_column_type_t)type));
 *      break;
 *  }
 *
 *  return 0;
 * }
 */
size_t temporal_field_size(uint8_t type, uint8_t decimals)
{
    switch (type)
    {
        case DRIZZLE_COLUMN_TYPE_YEAR:
            return 1;

        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_DATE:
            return 3;

        case DRIZZLE_COLUMN_TYPE_TIME2:
            return 3 + ((decimals + 1) / 2);

        case DRIZZLE_COLUMN_TYPE_DATETIME:
            // return length < 0 || length > 6 ? 8 : datetime_sizes[length];
            return 8;
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
            return 4;

        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
            return 4 + ((decimals + 1) / 2);

        case DRIZZLE_COLUMN_TYPE_DATETIME2:
            return 5 + ((decimals + 1) / 2);

        default:
            printf("Unknown field type: %d %s", type,
                   drizzle_column_type_str((drizzle_column_type_t) type));
            break;
    } // switch

    return 0;
} // temporal_field_size


/**
 * @brief Unpack a temporal value
 *
 * MariaDB and MySQL both store temporal values in a special format. This
 * function
 * unpacks them from the storage format and into a common, usable format.
 * @param type Column type
 * @param val Extracted packed value
 * @param tm Pointer where the unpacked temporal value is stored
 */
size_t unpack_temporal_value(drizzle_column_type_t type, uint8_t *ptr,
                             uint8_t *metadata, int length, struct tm *tm)
{
    switch (type)
    {
        case DRIZZLE_COLUMN_TYPE_YEAR:
            unpack_year(ptr, tm);
            break;

        case DRIZZLE_COLUMN_TYPE_DATETIME:
            unpack_datetime(ptr, length, tm);
            break;

        case DRIZZLE_COLUMN_TYPE_DATETIME2:
            unpack_datetime2(ptr, tm);
            break;

        case DRIZZLE_COLUMN_TYPE_TIME:
            unpack_time(ptr, tm);
            break;

        case DRIZZLE_COLUMN_TYPE_DATE:
            unpack_date(ptr, tm);
            break;

        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
            unpack_timestamp(ptr, tm);
            break;

        // DRIZZLE_RETURN_INVALID_CONVERSION
        // invalid column types for temporal unpacking
        case DRIZZLE_COLUMN_TYPE_BIT:
        case DRIZZLE_COLUMN_TYPE_BLOB:
        case DRIZZLE_COLUMN_TYPE_DECIMAL:
        case DRIZZLE_COLUMN_TYPE_DOUBLE:
        case DRIZZLE_COLUMN_TYPE_ENUM:
        case DRIZZLE_COLUMN_TYPE_FLOAT:
        case DRIZZLE_COLUMN_TYPE_GEOMETRY:
        case DRIZZLE_COLUMN_TYPE_INT24:
        case DRIZZLE_COLUMN_TYPE_LONG:
        case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
        case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
        case DRIZZLE_COLUMN_TYPE_NEWDATE:
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
//    case DRIZZLE_COLUMN_TYPE_NONE:
        case DRIZZLE_COLUMN_TYPE_NULL:
        case DRIZZLE_COLUMN_TYPE_SET:
        case DRIZZLE_COLUMN_TYPE_SHORT:
        case DRIZZLE_COLUMN_TYPE_STRING:
        case DRIZZLE_COLUMN_TYPE_TIME2:
        case DRIZZLE_COLUMN_TYPE_TINY:
        case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
        case DRIZZLE_COLUMN_TYPE_VARCHAR:
            break;

        default:
            assert(false);
            break;
    } // switch
      // return temporal_field_size(type, *metadata, length);
    return temporal_field_size(type, *metadata);
} // unpack_temporal_value


drizzle_field_datatype_t get_field_datatype(drizzle_column_type_t field_type)
{
    switch ( field_type )
    {
        case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
        case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
        case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
        case DRIZZLE_COLUMN_TYPE_BLOB:
            return DRIZZLE_FIELD_DATATYPE_BLOB;

        case DRIZZLE_COLUMN_TYPE_VARCHAR:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
        case DRIZZLE_COLUMN_TYPE_STRING:
            return DRIZZLE_FIELD_DATATYPE_STRING;


        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_TIME2:
        case DRIZZLE_COLUMN_TYPE_NEWDATE:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
        case DRIZZLE_COLUMN_TYPE_DATETIME:
        case DRIZZLE_COLUMN_TYPE_DATETIME2:
            return DRIZZLE_FIELD_DATATYPE_TEMPORAL;

        case DRIZZLE_COLUMN_TYPE_BIT:
            return DRIZZLE_FIELD_DATATYPE_BIT;

        case DRIZZLE_COLUMN_TYPE_FLOAT:
        case DRIZZLE_COLUMN_TYPE_DOUBLE:
        case DRIZZLE_COLUMN_TYPE_DECIMAL:
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
            return DRIZZLE_FIELD_DATATYPE_DECIMAL;

        case DRIZZLE_COLUMN_TYPE_TINY:
        case DRIZZLE_COLUMN_TYPE_YEAR:
        case DRIZZLE_COLUMN_TYPE_SHORT:
        case DRIZZLE_COLUMN_TYPE_INT24:
        case DRIZZLE_COLUMN_TYPE_LONG:
            return DRIZZLE_FIELD_DATATYPE_LONG;

        case DRIZZLE_COLUMN_TYPE_LONGLONG:
            return DRIZZLE_FIELD_DATATYPE_LONGLONG;

        case DRIZZLE_COLUMN_TYPE_NULL:
        case DRIZZLE_COLUMN_TYPE_ENUM:
        case DRIZZLE_COLUMN_TYPE_SET:
        case DRIZZLE_COLUMN_TYPE_GEOMETRY:
        default:
            return DRIZZLE_FIELD_DATATYPE_NONE;
    } // switch
      //
} // get_field_datatype
