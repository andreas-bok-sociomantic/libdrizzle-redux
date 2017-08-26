#include "config.h"
#include "src/common.h"

uint64_t drizzle_binlog_rbr_xid(drizzle_binlog_rbr_st *binlog_rbr)
{
    return binlog_rbr->xid_event.xid;
}

drizzle_binlog_tablemap_event_st * drizzle_binlog_rbr_st::get_tablemap_event(uint64_t table_id)
{
    if (tablemap_events.find(table_id) == tablemap_events.end())
    {
        tablemap_events.insert(std::make_pair(table_id, new drizzle_binlog_tablemap_event_st()));
    }

    return tablemap_events.find(table_id)->second;
}

drizzle_binlog_rows_event_st * drizzle_binlog_rbr_st::get_rows_event()
{
    rows_events.push_back(new drizzle_binlog_rows_event_st());
    return rows_events.back();
}
