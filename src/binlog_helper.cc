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


static bool bit_is_set(uint8_t *ptr, int columns, int current_column);
{
    if (current_column >= 8)
    {
        ptr += current_column / 8;
        current_column = current_column % 8;
    }

    return ((*ptr) & (1 << current_column));
}
