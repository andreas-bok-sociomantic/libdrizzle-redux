#pragma once

#include <map>

struct drizzle_binlog_rbr_st
{
    drizzle_binlog_xid_event_st xid_event;
    drizzle_binlog_query_event_st query_event;
    drizzle_binlog_tablemap_event_st *tablemap_events;
    drizzle_binlog_rows_event_st *rows_events;
    drizzle_binlog_st *binlog;

    uint64_t xid;

    drizzle_binlog_rbr_st() :
    tablemap_events(NULL),
    rows_events(NULL),
    binlog(NULL)
    {

    }
};
