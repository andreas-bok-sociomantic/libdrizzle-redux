#pragma once

struct drizzle_binlog_rows_event_st
{
    drizzle_binlog_event_st header;

    uint64_t table_id;
    uint64_t column_count;
    uint32_t field_metadata_len;
    uint8_t *column_type_def;
    uint8_t *field_metadata;
    uint64_t bitmap_size;

    drizzle_binlog_rows_event_st() :
        table_id(0),
        column_count(0),
        field_metadata_len(0),
        column_type_def(NULL),
        field_metadata(NULL),
        bitmap_size(0)
    {}
};
