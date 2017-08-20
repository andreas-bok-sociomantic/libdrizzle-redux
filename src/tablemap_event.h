#pragma once

struct drizzle_binlog_tablemap_event_st
{
    drizzle_binlog_event_st header;

    uint64_t table_id;
    uint8_t flags[2];
    char schema_name[DRIZZLE_MAX_DB_SIZE];
    char table_name[DRIZZLE_MAX_TABLE_SIZE];
    uint64_t column_count;
    drizzle_column_type_t *column_type_def;
    uint8_t *field_metadata;
    uint64_t field_metadata_len;
    uint8_t *null_bitmap;

    drizzle_binlog_tablemap_event_st() :
    table_id(0),
    column_count(0),
    column_type_def(NULL),
    field_metadata(NULL),
    field_metadata_len(0),
    null_bitmap(NULL)
    {}
};
