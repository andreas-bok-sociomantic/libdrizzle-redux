#include "config.h"
#include "src/common.h"

#include <iostream>

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

uint32_t ptr_dist(unsigned char *ptr1, unsigned char *ptr2)
{
    return ((const char *) ptr1 - (const char *) ptr2);
}
