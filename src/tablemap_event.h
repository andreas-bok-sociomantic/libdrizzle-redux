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
 * @file
 * @brief tablemap_event struct definition
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      tablemap_event struct definition
 *
 * @description
 * The first event used in Row Based Replication which declares how a table
 * that is about to be changed is defined
 *
 * @verbatim
 * Post-header:
 * if post_header_len == 6 {
 *     4              table id
 * } else {
 *     6              table id
 * }
 *     2              flags
 *
 * Payload:
 *     1              schema name length
 *     string         schema name
 *     1              [00]
 *     1              table name length
 *     string         table name
 *     1              [00]
 *     lenenc-int     column-count
 *     string.var_len [length=$column-count] column-def
 *     lenenc-str     column-meta-def
 *     n              NULL-bitmask, length: (column-count + 8) / 7
 * @endverbatim
 *
 * @sa:
 *     https://dev.mysql.com/doc/internals/en/table-map-event.html
 */
struct drizzle_binlog_tablemap_event_st
{
    /**
     *  Common header for all binlog events
     */
    drizzle_binlog_event_st header;

    /**
     * Unique id assigned to the table by the MySQL server
     */
    uint64_t table_id;

    /**
     * Reserved for future use
     */
    uint8_t flags[2];

    /**
     * The database name
     */
    char schema_name[DRIZZLE_MAX_DB_SIZE];

    /**
     * The table name
     */
    char table_name[DRIZZLE_MAX_TABLE_SIZE];

    /**
     * The number of columns in the table
     */
    uint64_t column_count;

    /**
     * An array of column types, one byte per column. To find the meanings of
     * these values, look at enum_field_types in the mysql_com.h header file.
     */
    drizzle_column_type_t *column_type_def;

    /**
     * The metadata block; see log_event.h for contents and format.
     */
    uint8_t *field_metadata;

    /**
     * Length of the metadata block
     */
    uint64_t field_metadata_len;

    /**
     * Bit-field indicating whether each column can be NULL, one bit per column
     * Example for 4 columns where column 2,3,4 can be null,
     * that NOT NULL is not set
     *  1  2  3  4
     *  NN  N  N  N
     *  1110
     *  0x0e
     *  00 00 11 10
     *
     *  \sa:
     *  https://dev.mysql.com/doc/internals/en/null-bitmap.html
     *  https://dev.mysql.com/doc/internals/en/binary-protocol-resultset-row.html
     */
    uint8_t *null_bitmap;

    /**
     * @brief      Constructor
     */
    drizzle_binlog_tablemap_event_st() :
        table_id(0),
        column_count(0),
        column_type_def(NULL),
        field_metadata(NULL),
        field_metadata_len(0),
        null_bitmap(NULL) {}

    ~drizzle_binlog_tablemap_event_st()
    {
        if(column_type_def!=NULL)
            free(column_type_def);
        if (field_metadata!=NULL)
            free(field_metadata);
        if (null_bitmap!=NULL)
            free(null_bitmap);
    }
};


/**
 * @brief      Parse a table event struct
 *
 * @param      event  a binlog event struct
 *
 * @return     Pointer to the parsed tablemap event struct
 */
drizzle_binlog_tablemap_event_st *drizzle_binlog_parse_tablemap_event(
    drizzle_binlog_event_st *event);

#ifdef __cplusplus
}
#endif
