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
 * @param ptr Pointer to start of bitfield
 * @param columns Number of columns (bits)
 * @param current_column Zero indexed column number
 * @return True if the bit is set
 */
DRIZZLE_API
bool bit_is_set(const unsigned char *ptr, int current_column);


/**
 * @brief      Get the protocol datatype for a column
 *
 * @param[in]  type  The type
 *
 * @return     { description_of_the_return_value }
 */
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

#ifdef __cplusplus
}
#endif
