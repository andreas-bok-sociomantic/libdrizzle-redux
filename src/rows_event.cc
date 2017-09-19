#include "config.h"
#include "src/common.h"

drizzle_binlog_rows_event_st *drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event)
{
    return event->binlog_rbr->get_rows_event();
}

drizzle_binlog_rows_event_st *drizzle_binlog_parse_rows_event(
    drizzle_binlog_event_st *event)
{
    auto rows_event = event->binlog_rbr->create_rows_event();
    assert(rows_event != NULL);
    set_event_header(&rows_event->header, event);

    // Get the table id
    rows_event->table_id = drizzle_get_byte6(event->data_ptr);
    event->data_ptr+=6;

    // Get the associated tablemap
    auto table_map_event = event->binlog_rbr->get_tablemap_event(rows_event->table_id);

    rows_event->column_type_def = (drizzle_column_type_t*)
        malloc(table_map_event->column_count + 1);
    memcpy(rows_event->column_type_def, event->data_ptr, table_map_event->column_count);
    event->data_ptr+=table_map_event->column_count;

    rows_event->field_metadata = (uint8_t*) malloc(table_map_event->field_metadata_len);
    memcpy(rows_event->field_metadata, event->data_ptr, table_map_event->field_metadata_len);
    event->data_ptr += table_map_event->field_metadata_len;

    strcpy(rows_event->table_name, table_map_event->table_name);

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


    event->binlog_rbr->add_table_row_mapping(rows_event);
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

const char* drizzle_binlog_rows_event_table_name(
    drizzle_binlog_rows_event_st *event)
{
    return event->table_name;
}
