
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
drizzle_binlog_event_st* drizzle_binlog_query_event_header(
    drizzle_binlog_xid_event_st* event);


/**
 * @brief      Get a query event
 *
 * @param      event  a drizzle_binlog_event_st
 *
 * @return     a drizzle_binlog_query_event_st
 */
DRIZZLE_API
drizzle_binlog_query_event_st *drizzle_binlog_get_query_event(
    drizzle_binlog_event_st *event);

/**
 * @brief      { function_description }
 *
 * @return     { description_of_the_return_value }
 */
DRIZZLE_API
uint32_t drizzle_binlog_query_event_slave_proxy_id(
    drizzle_binlog_query_event_st* query_event);

/**
 * @brief      { item_description }
 *
 * @return     Time in seconds
 */
DRIZZLE_API
uint32_t drizzle_binlog_query_event_execution_time(
    drizzle_binlog_query_event_st* query_event);

/**
 * @brief      The error code resulting from execution of the statement on
 *             the master
 *
 * @return     MySQL error code
 */
DRIZZLE_API
uint16_t drizzle_binlog_query_event_error_code(
    drizzle_binlog_query_event_st* query_event);

/**
 * @brief      The size of the status variable block in bytes
 *
 * @return     size of the status variables in bytes
 */
DRIZZLE_API
uint16_t drizzle_binlog_query_event_status_vars_length(
    drizzle_binlog_query_event_st* query_event);

/**
 * @brief      Zero or more status variables. Each status variable consists
 *             of one byte code identifying the variable stored, followed by
 *             the value of the variable.
 *
 * @return     array of status vars
 */
DRIZZLE_API
uint8_t* drizzle_binlog_query_event_status_vars(
    drizzle_binlog_query_event_st* query_event);

/**
 * @brief      The name of the schema which casused the binlog event
 *
 * @return     { description_of_the_return_value }
 */
DRIZZLE_API
unsigned char* drizzle_binlog_query_event_schema(
    drizzle_binlog_query_event_st* query_event);

/**
 * @brief      The SQL statement which was executed
 *
 * @return     SQL query string
 */
DRIZZLE_API
unsigned char* drizzle_binlog_query_event_query(
    drizzle_binlog_query_event_st* query_event);



#ifdef __cplusplus
}
#endif

