#pragma once

struct drizzle_binlog_query_event_st
{
    drizzle_binlog_event_st header;

    uint32_t slave_proxy_id;
    uint32_t execution_time;
    uint16_t error_code;
    uint16_t status_vars_length;
    uint8_t *status_vars;
    unsigned char schema[DRIZZLE_MAX_DB_SIZE];
    unsigned char *query;

    drizzle_binlog_query_event_st() :
        slave_proxy_id(0),
        execution_time(0),
        error_code(0),
        status_vars_length(0),
        status_vars(NULL),
        query(NULL)
    {}
};


drizzle_binlog_query_event_st *drizzle_binlog_parse_query_event(
    drizzle_binlog_event_st *event);
