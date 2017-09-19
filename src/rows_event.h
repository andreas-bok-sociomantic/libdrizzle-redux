#pragma once

#include <vector>


struct drizzle_binlog_rows_event_st
{
    /**
     *
     * iterate rows
     *      iterate columns
     *          column by index
     *      row by index
     *
     *
     *
     *
     * drizzle_binlog_row_st
     *  drizzle_column_st *next
     *  drizzle_column_st *next
     *  uint column_current;
     *
     *
     *  drizzle_binlog_rows_event_st
     *    drizzle_binlog_row_st *next
     *    drizzle_binlog_row_st *prev
     *
     *
     * combine result client and column client, field
     */

    drizzle_binlog_event_st header;

    uint64_t table_id;
    char table_name[DRIZZLE_MAX_TABLE_SIZE];
    uint64_t column_count;
    uint32_t field_metadata_len;
    drizzle_column_type_t *column_type_def;
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

struct drizzle_binlog_row_events_st
{
    typedef std::vector<drizzle_binlog_rows_event_st*> vec_row_events;

    vec_row_events rows_events;
    bool active;
    vec_row_events::iterator it_;
    size_t count_;
    drizzle_binlog_row_events_st() :
    active(false)
    {}

    void reset()
    {
        active = false;
    }

    vec_row_events::iterator it()
    {
        if (!active)
        {
            it_ = rows_events.begin();
        }

        return it_;
    }

    drizzle_binlog_rows_event_st* curr()
    {
        return *it_;
    }

    drizzle_binlog_rows_event_st* at(size_t index)
    {
        return index < rows_events.size() ? rows_events.at(index) : NULL;
    }

    void operator++()
    {
        it_++;
    }

    void operator--(int)
    {
        it_--;
    }
};


/**
 * @brief      Parse a rows event from binary stream
 *
 * @param      event  a binlog event struct
 *
 * @return     Pointer to the parsed rows event struct
 */
drizzle_binlog_rows_event_st *drizzle_binlog_parse_rows_event(
    drizzle_binlog_event_st *event);

