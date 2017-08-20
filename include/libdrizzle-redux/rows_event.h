#pragma once

#ifdef __cplusplus
extern "C" {
#endif

drizzle_binlog_rows_event_st *drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event,
    drizzle_binlog_tablemap_event_st *table_map_event);

/**
 * @brief      The id of the table of assigned by the MySQL
 *             server
 *
 * @return     A table id
 */
DRIZZLE_API
uint64_t drizzle_binlog_rows_event_table_id(
    drizzle_binlog_event_st *event);

/**
 * @brief      The number of columns in the associated table
 *
 * @return     Number of columns
 */
DRIZZLE_API
uint64_t drizzle_binlog_rows_event_column_count(
    drizzle_binlog_event_st *event);

#ifdef __cplusplus
}
#endif
