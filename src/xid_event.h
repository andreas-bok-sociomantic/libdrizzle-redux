#pragma once

struct drizzle_binlog_xid_event_st
{
    drizzle_binlog_event_st header;
    /**
     * The xid id for the transaction
     */
    uint64_t xid;

    drizzle_binlog_xid_event_st() :
    xid(0)
    {}
};