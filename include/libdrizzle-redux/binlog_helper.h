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


#ifdef __cplusplus
}
#endif
