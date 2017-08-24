#include "config.h"
#include "src/common.h"

/**
 * Get the header of a binlog event
 *
 * @param      event  a xid event structure
 *
 * @return     pointer to the event's header
 */
drizzle_binlog_event_st* drizzle_binlog_query_event_header(
    drizzle_binlog_query_event_st* event)
{
    return &event->header;
}


drizzle_binlog_query_event_st *drizzle_binlog_get_query_event(
    drizzle_binlog_event_st *event)
{
    auto query_event = &event->binlog_rbr->query_event;
    set_event_header(&query_event->header, event);

    query_event->slave_proxy_id = (uint32_t) drizzle_get_byte4(event->data_ptr);
    event->data_ptr += 4;

    query_event->execution_time = (uint32_t) drizzle_get_byte4(event->data_ptr);
    event->data_ptr += 4;

    uint8_t schema_length = event->data_ptr[0];
    event->data_ptr++;

    query_event->error_code = drizzle_get_byte2(event->data_ptr);
    event->data_ptr += 2;

    uint64_t value_len = drizzle_get_byte2(event->data_ptr);
    event->data_ptr += 2;
    query_event->status_vars = (uint8_t*) realloc(query_event->status_vars, value_len);
    memcpy(query_event->status_vars, event->data_ptr, value_len);
    event->data_ptr += value_len;

    memcpy(&query_event->schema, event->data_ptr, schema_length);
    query_event->schema[schema_length] = '\0';
    event->data_ptr += schema_length;

    event->data_ptr++;

    value_len = drizzle_binlog_event_available_bytes(event) - 4;
    query_event->query = (unsigned char*) realloc(query_event->query,
        value_len + 1);
    memcpy(query_event->query, event->data_ptr, value_len);

    query_event->query[value_len] = '\0';
    event->data_ptr += value_len + 4;
    return query_event;
}

uint32_t drizzle_binlog_query_event_slave_proxy_id(
    drizzle_binlog_query_event_st* query_event)
{
    return query_event->slave_proxy_id;
}

uint32_t drizzle_binlog_query_event_execution_time(
    drizzle_binlog_query_event_st* query_event)
{
    return query_event->execution_time;
}

uint16_t drizzle_binlog_query_event_error_code(
    drizzle_binlog_query_event_st* query_event)
{
    return query_event->error_code;
}

uint16_t drizzle_binlog_query_event_status_vars_length(
    drizzle_binlog_query_event_st* query_event)
{
    return query_event->status_vars_length;
}

uint8_t *drizzle_binlog_query_event_status_vars(
    drizzle_binlog_query_event_st* query_event)
{
    return query_event->status_vars;
}

unsigned char *drizzle_binlog_query_event_schema(
    drizzle_binlog_query_event_st* query_event)
{
    return query_event->schema;
}

unsigned char *drizzle_binlog_query_event_query(
    drizzle_binlog_query_event_st* query_event)
{
    return query_event->query;
}

// unsigned char *drizzle_binlog_query_event_status_var_length(
//      uint8_t *status_vars, drizzle_query_event_status_var_codes_t *status_code)
//  {
//     uint32_t length = 0;
//     switch (status_var)
//     {
//         case DRIZZLE_QUERY_EVENT_STATUS_VAR_FLAGS2:
//             length = 4;
//             break;
//         case DRIZZLE_QUERY_EVENT_STATUS_VAR_SQL_MODE:
//             length = 8;
//             break;
//         case DRIZZLE_QUERY_EVENT_STATUS_VAR_CATALOG:
//             length = 0;
//             break;
//         case DRIZZLE_QUERY_EVENT_STATUS_VAR_AUTO_INCREMENT:
//             length = 2;
//             break;
//         case DRIZZLE_QUERY_EVENT_STATUS_VAR_CHARSET:
//             length = 0;
//             break;
//         case DRIZZLE_QUERY_EVENT_STATUS_VAR_TIME_ZONE:
//             length = 0;
//             break;
//         case DRIZZLE_QUERY_EVENT_STATUS_VAR_CATALOG_NZ:
//             length = 1;
//             break;
//         case DRIZZLE_QUERY_EVENT_STATUS_VAR_LC_TIME_NAMES:
//             length = 2;
//             break;
//         case DRIZZLE_QUERY_EVENT_STATUS_VAR_CHARSET_DATABASE:
//             length = 2;
//             break;
//         case DRIZZLE_QUERY_EVENT_STATUS_VAR_TABLE_MAP_FOR_UPDATE:
//             length = 8;
//             break;
//     }
//  }
