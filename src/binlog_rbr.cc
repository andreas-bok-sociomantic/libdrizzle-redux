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
    if (rows_count++ > rows_events.size())
    {
        rows_events.push_back(new drizzle_binlog_rows_event_st());
    }

    return rows_events.at(rows_count);
}


void drizzle_binlog_rbr_st::add_binlog_event(drizzle_binlog_event_st* event)
{
    if (event->type == DRIZZLE_EVENT_TYPE_XID)
    {
        drizzle_binlog_get_xid_event(event);
    }
    else if (event->type == DRIZZLE_EVENT_TYPE_QUERY)
    {
        drizzle_binlog_get_query_event(event);
    }
    else
    {
        return;
    }
}

void drizzle_binlog_rbr_st::reset()
{
    rows_events.clear();
    rows_count = 0;
    tablemap_events.clear();
    current_row = NULL;
}
