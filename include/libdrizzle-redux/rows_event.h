#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief      Get a rows event struct from the binlog stream
 *
 * @param      event  a binlog event struct

 *
 * @return     A pointer to a rows event struct
 */
DRIZZLE_API
drizzle_binlog_rows_event_st *drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event);

/**
 * @brief      The id of the table of assigned by the MySQL
 *             server
 *
 * @param      event  a binlog event struct
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
const char* drizzle_binlog_rows_event_table_name(
    drizzle_binlog_rows_event_st *event);

/**
 * @brief      The number of columns in the associated table
 *
 * @param      event  a binlog event struct
 *
 * @return     Number of columns
 */
DRIZZLE_API
uint64_t drizzle_binlog_rows_event_column_count(
    drizzle_binlog_rows_event_st *event);

#ifdef __cplusplus
}
#endif
