#include "config.h"
#include "src/common.h"

drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_st::get_tablemap_event(uint64_t table_id)
{
    if (tablemap_events.find(table_id) == tablemap_events.end())
    {
        tablemap_events.insert(std::make_pair(table_id, new drizzle_binlog_tablemap_event_st()));
    }

    return tablemap_events.find(table_id)->second;
}

drizzle_binlog_rows_event_st *drizzle_binlog_rbr_st::get_rows_event()
{
    if (row_events_count_++ > rows_events.size())
    {
        rows_events.push_back(new drizzle_binlog_rows_event_st());
    }

    return rows_events.at(row_events_count_);
}


void drizzle_binlog_rbr_st::add_binlog_event(drizzle_binlog_event_st* event)
{
    if (event->type == DRIZZLE_EVENT_TYPE_XID)
    {
        drizzle_binlog_get_xid_event(event);
        if (binlog_rbr_fn != NULL)
        {
            binlog_rbr_fn(this, binlog->binlog_context);
        }
    }
    else if (event->type == DRIZZLE_EVENT_TYPE_QUERY)
    {
        drizzle_binlog_get_query_event(event);
    }
    else if (event->type == DRIZZLE_EVENT_TYPE_V2_WRITE_ROWS ||
        event->type == DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS ||
        event->type == DRIZZLE_EVENT_TYPE_V2_DELETE_ROWS)
    {
        drizzle_binlog_get_rows_event(event);
    }
    else if (event->type == DRIZZLE_EVENT_TYPE_TABLE_MAP)
    {
        drizzle_binlog_get_tablemap_event(event);
    }
    else
    {
        return;
    }
}

void drizzle_binlog_rbr_st::reset(bool free_rows)
{
    if (free_rows)
    {
        rows_events.clear();
    }

    row_it = rows_events.end();
    row_events_count_ = 0;
    tablemap_events.clear();
}

size_t drizzle_binlog_rbr_st::row_events_count()
{
    return row_events_count_;
}
