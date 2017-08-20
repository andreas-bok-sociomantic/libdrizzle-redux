#include "config.h"
#include "src/common.h"

drizzle_binlog_rows_event_st *drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event,
    drizzle_binlog_tablemap_event_st *table_map_event)
{
    auto rows_event = new drizzle_binlog_rows_event_st();
    set_event_header(&rows_event->header, event);

    rows_event->column_type_def = (drizzle_column_type_t*) malloc(table_map_event->column_count);
    memcpy(&rows_event->column_type_def, event->data_ptr, table_map_event->column_count);
    event->data_ptr+=table_map_event->column_count;

    rows_event->field_metadata = (uint8_t*) malloc(table_map_event->field_metadata_len);
    memcpy(&rows_event->field_metadata, event->data_ptr, table_map_event->field_metadata_len);
    event->data_ptr += table_map_event->field_metadata_len;
    // metadata definition

    strcpy(rows_event->table_name, table_map_event->table_name);

    rows_event->table_id = drizzle_get_byte6(event->data_ptr);
    event->data_ptr+=6;

    uint16_t flags = drizzle_get_byte2(event->data_ptr);
    event->data_ptr+=2;

    if ( rows_event->table_id == 0x00ffffff && flags == (1 << 0))
    {
        return NULL;
    }

    if (drizzle_binlog_rows_event_version(event->type) == 2)
    {
        auto len = drizzle_get_byte2(event->data_ptr);
        if ( len > 2 )
        {
            event->data_ptr+=2;
        }
    }

    // column_count
    rows_event->column_count = drizzle_binlog_get_encoded_len(event);

    rows_event->bitmap_size = (rows_event->column_count + 7)/8;
    return rows_event;
}

uint64_t drizzle_binlog_rows_event_table_id(
    drizzle_binlog_rows_event_st *event)
{
    return event->table_id;
}

uint64_t drizzle_binlog_rows_event_column_count(
    drizzle_binlog_rows_event_st *event)
{
    return event->column_count;
}