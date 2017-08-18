#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Get a binlog rows event
 *
 * @param      event            A drizzle_binlog_event_st
 * @param      table_map_event  Pointer to a table_map_event
 *
 * @return     Return a drizzle_binlog_rows_event_st
 */
DRIZZLE_API
drizzle_binlog_rows_event_st *drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event,
    drizzle_binlog_tablemap_event_st *table_map_event);


/**
 * @brief      Get the XID event from a binlog event group
 *
 * @return     Pointer to a drizzle_binlog_xid_event struct
 */
DRIZZLE_API
drizzle_binlog_xid_event_st *drizzle_binlog_rbr_get_xid_event(
    drizzle_binlog_rbr_st* rbr);

/**
 * @brief      Get the Query event from a binlog event group
 *
 * @return     Pointer to a drizzle_binlog_query_event
 */
DRIZZLE_API
drizzle_binlog_query_event_st *drizzle_binlog_rbr_get_query_event(
    drizzle_binlog_rbr_st* rbr);

/**
 * @brief      Set the client callback function to receive row based
 *             replication event group notifications
 *
 * @param      binlog_st  An initialized binlog structure
 * @param      rbr_fn     The function callback defined in (drizzle_binlog_rbr_fn)()
 * @param      context    A pointer to user data which will be used for the
 *                        callback functions
 */
DRIZZLE_API
drizzle_return_t drizzle_binlog_set_rbr_fn(drizzle_binlog_st *binlog,
    drizzle_binlog_rbr_fn *rbr_fn, void *context);


#ifdef  __cplusplus
}
#endif
