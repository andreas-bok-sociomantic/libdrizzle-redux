#pragma once

#include <unordered_map>

struct drizzle_binlog_rbr_st
{
    drizzle_binlog_xid_event_st xid_event;
    drizzle_binlog_query_event_st query_event;
    typedef std::unordered_map<uint64_t, drizzle_binlog_tablemap_event_st*>
        map_tablemap_events;
    map_tablemap_events tablemap_events;
    drizzle_binlog_rows_event_st *rows_events;
    drizzle_binlog_st *binlog;

    uint64_t xid;

    drizzle_binlog_rbr_st() :
    rows_events(NULL),
    binlog(NULL)
    {
    }

    ~drizzle_binlog_rbr_st()
    {
    }
};