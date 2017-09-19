#pragma once

struct drizzle_binlog_xid_event_st
{
    /**
     *  Common header for all binlog events
     */
    drizzle_binlog_event_st header;

    /**
     * The xid id for the transaction
     */
    uint64_t xid;

    /**
     * @brief      constructor
     */
    drizzle_binlog_xid_event_st() :
    xid(0)
    {}
};


/**
 * @brief      Parse a xid event from binlog stream
 *
 * @param      event  A binlog event struct
 *
 * @return     pointer to a xid event struct
 */
drizzle_binlog_xid_event_st *drizzle_binlog_parse_xid_event(
    drizzle_binlog_event_st *event);
