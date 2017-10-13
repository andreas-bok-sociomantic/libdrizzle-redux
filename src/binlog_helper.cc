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


void set_event_header(drizzle_binlog_event_st* dst,
    drizzle_binlog_event_st* src)
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
 * @brief Check if a bit is set
 *
 * @param ptr Pointer to start of bitfield
 * @param columns Number of columns (bits)
 * @param current_column Zero indexed column number
 * @return True if the bit is set
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
 * @param type Column type
 * @return True if column is either ENUM or SET
 */
bool fixed_string_is_enum(drizzle_column_type_t type)
{
    return type == DRIZZLE_COLUMN_TYPE_ENUM || type == DRIZZLE_COLUMN_TYPE_SET;
}

/**
 * @brief Check if the column is a string type column
 *
 * @param type Type of the column
 * @return True if the column is a string type column
 * @see mxs_lestr_consume
 */
bool column_is_fixed_string(drizzle_column_type_t type)
{
    return type == DRIZZLE_COLUMN_TYPE_VAR_STRING;
}


/**
 * @brief Unpack an ENUM or SET field
 * @param ptr Pointer to packed value
 * @param metadata Pointer to field metadata
 * @return Length of the processed field in bytes
 */
size_t unpack_enum(uint8_t *ptr, uint8_t *metadata, uint8_t *dest)
{
    memcpy(dest, ptr, metadata[1]);
    return metadata[1];
}


drizzle_column_protocol_datatype_t column_protocol_datatype(drizzle_column_type_t type)
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
        case DRIZZLE_COLUMN_TYPE_TIME2 :
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2 :
        case DRIZZLE_COLUMN_TYPE_DATETIME:
        case DRIZZLE_COLUMN_TYPE_DATETIME2 :
            return TEMPORAL;

        // DRIZZLE_COLUMN_TYPE_ENUM and DRIZZLE_COLUMN_TYPE_SET are
        // packed as a DRIZZLE_COLUMN_TYPE_STRING and cannot appear as a
        // type in the fixed header. Their respective types must be
        // extracted from the column metadata
        case DRIZZLE_COLUMN_TYPE_STRING:
            return FIXED_STRING;

        case DRIZZLE_COLUMN_TYPE_TINY:
        case DRIZZLE_COLUMN_TYPE_SHORT:
        case DRIZZLE_COLUMN_TYPE_INT24 :
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
    }
}


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
        case DRIZZLE_COLUMN_TYPE_TIME2 :
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2 :
        case DRIZZLE_COLUMN_TYPE_DATETIME2 :
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
    }
}
