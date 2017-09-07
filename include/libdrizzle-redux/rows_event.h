#pragma once

#ifdef __cplusplus
extern "C" {
#endif

DRIZZLE_API
drizzle_binlog_rows_event_st *drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event);

/**
 * @brief      The id of the table of assigned by the MySQL
 *             server
 *
 * @return     A table id
 */
DRIZZLE_API
uint64_t drizzle_binlog_rows_event_table_id(drizzle_binlog_rows_event_st *event);

/**
 * @brief      The table name associated with the rows event
 *
 * @param[in]  event The event from the binlog stream
 *
 * @return     The table name
 */
DRIZZLE_API
const char* drizzle_binlog_rows_event_table_name(drizzle_binlog_rows_event_st *event);

/**
 * @brief      The number of columns in the associated table
 *
 * @return     Number of columns
 */
DRIZZLE_API
uint64_t drizzle_binlog_rows_event_column_count(drizzle_binlog_rows_event_st *event);

#ifdef __cplusplus
}
#endif
