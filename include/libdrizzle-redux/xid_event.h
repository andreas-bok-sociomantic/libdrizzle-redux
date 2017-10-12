#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Get the header of a binlog event
 *
 * @param      event  a xid event structure
 *
 * @return     pointer to the event's header
 */
DRIZZLE_API
drizzle_binlog_event_st* drizzle_binlog_xid_event_header(
    drizzle_binlog_xid_event_st* event);


/**
 * Get the xid id for the transaction
 *
 * @return     Return the xid for the transaction
 */
DRIZZLE_API
uint64_t drizzle_binlog_xid_event_xid(const drizzle_binlog_xid_event_st* event);


/**
 * @brief      Get a xid event
 *
 * @param      event drizzle_binlog_event_st
 *
 * @return     a drizzle_binlog_xid_event_st
 */
DRIZZLE_API
drizzle_binlog_xid_event_st *drizzle_binlog_get_xid_event(
    drizzle_binlog_event_st *event);


#ifdef __cplusplus
}
#endif

