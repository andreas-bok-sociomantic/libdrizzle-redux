/*
 * Drizzle Client & Protocol Library
 *
 * Copyright: Copyright (c) 2017 sociomantic labs GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 *     * The names of its contributors may not be used to endorse or
 * promote products derived from this software without specific prior
 * written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

/**
 * @brief       QUERY_EVENT
 *
 * @detailed
 * Written when an updating statement is done
 *
 * @verbatim
 * Post-header:
 *     4              slave_proxy_id
 *     4              execution time
 *     1              schema length
 *     2              error-code
 * if binlog-version ≥ 4 {
 *     2              status-vars length
 * }
 *
 * Payload:
 *     string[$len]   status-vars
 *     string[$len]   schema
 *     1              [00]
 *     string[EOF]    query
 * @endverbatim
 *
 * @sa:
 *     https://dev.mysql.com/doc/internals/en/query-event.html
 */
struct drizzle_binlog_query_event_st
{
    /**
     *  Common header for all binlog events
     */
    drizzle_binlog_event_st header;

    /**
     * The ID of the thread that issued this statement.
     * Needed for temporary tables. This is also useful for a database admin
     * for knowing who did what on the master
     */
    uint32_t slave_proxy_id;

    /**
     * The time in seconds that the statement took to execute.
     */
    uint32_t execution_time;

    /**
     * The error code resulting from execution of the statement on the master
     */
    uint16_t error_code;

    /**
     * The length of the status variable block
     */
    uint16_t status_vars_length;

    /**
     * Zero or more status variables. Each status variable consists of one
     * byte code identifying the variable stored, followed by the value of the
     * variable.
     */
    uint8_t *status_vars;

    /**
     * The default database name
     */
    unsigned char schema[DRIZZLE_MAX_DB_SIZE];

    /**
     * The SQL statement which was executed
     */
    unsigned char *query;

    /**
     * @brief      Constructor
     */
    drizzle_binlog_query_event_st() :
        slave_proxy_id(0),
        execution_time(0),
        error_code(0),
        status_vars_length(0),
        status_vars(NULL),
        query(NULL)
    {}

    ~drizzle_binlog_query_event_st()
    {
        if (status_vars != NULL)
            free(status_vars);
        if (query!=NULL)
            free(query);
    }
};



/**
 * @brief      Parse a query event from binlog stream
 *
 * @param      event  a binlog event struct
 *
 * @return     Pointer to the parsed query event
 */
drizzle_binlog_query_event_st *drizzle_binlog_parse_query_event(
    drizzle_binlog_event_st *event);
