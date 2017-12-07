#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Check if a binlog event is a rows event
 *
 * @param[in]  event_type  a binlog event type
 *
 * @return     true if it is a rows event, false otherwise
 */
DRIZZLE_API
bool drizzle_binlog_is_rows_event(const drizzle_binlog_event_types_t event_type);


/**
 * Check if a bit is set
 *
 * @param      ptr             Pointer to start of bitfield
 * @param      current_column  Zero indexed column number
 *
 * @return     True if the bit is set
 */
DRIZZLE_API
bool bit_is_set(const unsigned char *ptr, int current_column);


/**
 * @brief      Get the protocol datatype for a column
 *
 * @param[in]  type  The type
 *
 * @return     A protocol datatype
 */
DRIZZLE_API
drizzle_column_protocol_datatype_t column_protocol_datatype(drizzle_column_type_t type);

/**
 * @brief      Get the version of the RBR replication Rows Event
 *
 * @param[in]  column_type  a column type
 *
 * @return     Return either 1 or 2 or 0 if column type wasn't found
 */
DRIZZLE_API
uint8_t drizzle_binlog_rows_event_version(
    drizzle_binlog_event_types_t column_type);


DRIZZLE_API
/**
 * @brief      Determines if rows event struct is an update event, ie. a SQL
 *             UPDATE ... statement incurred the binlog event to be written
 *
 * @param      rows_event  The rows event
 *
 * @return     True if the event is a update rows, False otherwise.
 */
bool is_rows_update_event(drizzle_binlog_rows_event_st *rows_event);

#ifdef __cplusplus
}
#endif
