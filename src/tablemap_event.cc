#include "config.h"
#include "src/common.h"

drizzle_binlog_tablemap_event_st *drizzle_binlog_get_tablemap_event(
    drizzle_binlog_event_st *event)
{
    // check if a tablemap event with that table id exists in the RBR
    // If so skip parsing and return the pointer to that element.
    // Otherwise create get a new tablemap event from the RBR, parse it and
    // return that
    uint64_t table_id = drizzle_get_byte6(event->data_ptr);
    auto tablemap_event = event->binlog_rbr->get_tablemap_event(table_id);
    set_event_header(&tablemap_event->header, event);

    tablemap_event->table_id = table_id;
    event->data_ptr+=6;

    memcpy(&tablemap_event->flags, event->data_ptr, 2);
    event->data_ptr+=2;

    uint8_t len_enc = event->data_ptr[0];
    memcpy(&tablemap_event->schema_name, event->data_ptr+1, len_enc);
    tablemap_event->schema_name[len_enc] = '\0';
    event->data_ptr+=len_enc + 1;

    event->data_ptr++;

    len_enc = event->data_ptr[0];
    memcpy(&tablemap_event->table_name, event->data_ptr+1, len_enc);
    tablemap_event->table_name[len_enc] = '\0';
    event->data_ptr+=len_enc + 1;

    event->data_ptr++;

    tablemap_event->column_count = drizzle_binlog_get_encoded_len(event);

    tablemap_event->column_type_def =
        (drizzle_column_type_t*) malloc(tablemap_event->column_count);
    memcpy(&tablemap_event->column_type_def, event->data_ptr,
        tablemap_event->column_count);
    event->data_ptr+=tablemap_event->column_count;

    len_enc = drizzle_binlog_get_encoded_len(event);
    tablemap_event->field_metadata_len = len_enc + 1;
    tablemap_event->field_metadata =
        (uint8_t*) malloc(tablemap_event->field_metadata_len);
    memcpy(&tablemap_event->field_metadata, event->data_ptr, len_enc);
    event->data_ptr+=len_enc;

    len_enc = (uint32_t) (tablemap_event->column_count + 7) / 8;
    tablemap_event->null_bitmap = (uint8_t*)malloc(len_enc);
    memcpy(&tablemap_event->null_bitmap, event->data_ptr, len_enc);
    event->data_ptr+=len_enc;

    return tablemap_event;
}



uint64_t drizzle_binlog_tablemap_event_table_id(
	drizzle_binlog_tablemap_event_st* event)
{
    return event->table_id;
}

const char *drizzle_binlog_tablemap_event_schema_name(
	drizzle_binlog_tablemap_event_st* event)
{
    return event->schema_name;
}

const char *drizzle_binlog_tablemap_event_table_name(
	drizzle_binlog_tablemap_event_st* event)
{
    return event->table_name;
}

uint64_t drizzle_binlog_tablemap_event_column_count(
	drizzle_binlog_tablemap_event_st* event)
{
    return event->column_count;
}

drizzle_column_type_t *drizzle_binlog_tablemap_event_column_type_def(
	drizzle_binlog_tablemap_event_st* event)
{
    return event->column_type_def;
}

drizzle_column_type_t drizzle_binlog_tablemap_event_column_type(
    drizzle_binlog_tablemap_event_st* event, uint32_t column_index,
    drizzle_return_t* ret)
{
    if (column_index >= event->column_count )
    {
        *ret = DRIZZLE_RETURN_INVALID_ARGUMENT;
        return DRIZZLE_COLUMN_TYPE_NONE;
    }

    return event->column_type_def[column_index];
}

uint8_t *drizzle_binlog_tablemap_event_field_metadata(
	drizzle_binlog_tablemap_event_st* event)
{
    return event->field_metadata;
}

uint64_t drizzle_binlog_tablemap_event_field_metadata_len(
	drizzle_binlog_tablemap_event_st* event)
{
    return event->field_metadata_len;
}

uint8_t *drizzle_binlog_tablemap_event_null_bitmap(
	drizzle_binlog_tablemap_event_st* event)
{
    return event->null_bitmap;
}
