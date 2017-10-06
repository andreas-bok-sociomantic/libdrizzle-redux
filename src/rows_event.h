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

#include <vector>


/**
 * @brief      Rows_event event
 *
 * @detailed
 * Implementation of rows events version 4 which is written by MySQL 5.0 and up
 *
 * Three basic kinds of ROWS_EVENT exist:
 *
 * @verbatim
 * +=========================================================================+
 * | Event             | SQL Cmd | Contents                                  |
 * +-------------------+---------+-------------------------------------------+
 * | WRITE_ROWS_EVENT  | INSERT  | the row data to insert                    |
 * +-------------------+---------+-------------------------------------------+
 * | DELETE_ROWS_EVENT | DELETE  | as much data as needed to identify a row  |
 * +-------------------+---------+-------------------------------------------+
 * | UPDATE_ROWS_EVENT | UPDATE  | as much data as needed to identify a row  |
 * |                   |         | + the data to change                      |
 * +=========================================================================+
 *
 * Post-header:
 * if post_header_len == 6 {
 *     4                  table id
 * } else {
 *     6                  table id
 * }
 *     2                  flags
 * if version == 2 {
 *     2                  extra-data-length
 *     string.var_len     extra-data
 * }
 *
 * Payload:
 *     lenenc_int         number of columns
 *     string.var_len     columns-present-bitmap1, length: (num of columns+7)/8
 * if UPDATE_ROWS_EVENTv1 or v2 {
 *     string.var_len     columns-present-bitmap2, length: (num of columns+7)/8
 * }
 *
 * Rows:
 *     string.var_len     nul-bitmap,
 *                        length (bits set in 'columns-present-bitmap1'+7)/8
 *     string.var_len     value of each field as defined in table-map
 * if UPDATE_ROWS_EVENTv1 or v2 {
 *     string.var_len     nul-bitmap,
 *                        length (bits set in 'columns-present-bitmap2'+7)/8
 *     string.var_len     value of each field as defined in table-map
 * }
 *     ... repeat rows until event-end
 * @endverbatim
 *
 * \sa: https://dev.mysql.com/doc/internals/en/rows-event.html
 */

struct drizzle_binlog_column_value_st
{
    uint8_t *metadata;
    unsigned char *data_ptr;
    drizzle_column_type_t type;

    drizzle_binlog_column_value_st() :
        metadata(NULL),
        data_ptr(NULL),
        type(DRIZZLE_COLUMN_TYPE_NONE)
    {}
};

struct drizzle_binlog_row_st
{
    drizzle_binlog_column_value_st *column_values_before;
    drizzle_binlog_column_value_st *column_values_after;

    drizzle_binlog_row_st() :
        column_values_before(NULL),
        column_values_after(NULL)
        {}
};

struct drizzle_binlog_rows_event_st
{
    /**
     *
     * iterate rows
     *      iterate columns
     *          column by index
     *      row by index
     *
     *
     *
     *
     * drizzle_binlog_row_st
     *  drizzle_column_st *next
     *  drizzle_column_st *next
     *  uint column_current;
     *
     *
     *  drizzle_binlog_rows_event_st
     *    drizzle_binlog_row_st *next
     *    drizzle_binlog_row_st *prev
     *
     *
     * combine result client and column client, field
     */

    /**
     *  Common header for all binlog events
     */
    drizzle_binlog_event_st header;

    /**
     * Id of the associated table
     */
    uint64_t table_id;

    /**
     * The table name
     */
    char table_name[DRIZZLE_MAX_TABLE_SIZE];

    /**
     * Number of columns in the row
     */
    uint64_t column_count;

    /**
     * Length of the metadata block
     */
    uint32_t field_metadata_len;

    /**
     * Array defining the type for each column present in the result
     */
    drizzle_column_type_t *column_type_def;

    /**
     * Array of type-specific metadata for each column
     */
    uint8_t *field_metadata;

    /**
     * Number of bytes required to represent the columns present bitmap
     */
    uint64_t bitmap_size;

    /**
     * List of parsed rows
     */
    drizzle_binlog_row_st *rows;

    /**
     * @brief      Constructor
     */
    drizzle_binlog_rows_event_st() :
        table_id(0),
        column_count(0),
        field_metadata_len(0),
        column_type_def(NULL),
        field_metadata(NULL),
        bitmap_size(0)
    {}

    ~drizzle_binlog_rows_event_st()
    {
        printf("called drizzle_binlog_rows_event_st destructor\n");
        if (column_type_def != NULL)
            free(column_type_def);
        if (field_metadata != NULL)
            free(field_metadata);
    }
};


struct drizzle_binlog_row_events_st
{
    typedef std::vector<drizzle_binlog_rows_event_st*> vec_row_events;

    vec_row_events rows_events;
    bool active;
    vec_row_events::iterator it_;
    size_t count_;
    drizzle_binlog_row_events_st() :
        active(false)
    {}

    void reset()
    {
        active = false;
    }

    vec_row_events::iterator it()
    {
        if (!active)
        {
            it_ = rows_events.begin();
        }

        return it_;
    }

    drizzle_binlog_rows_event_st* curr()
    {
        return *it_;
    }

    drizzle_binlog_rows_event_st* at(size_t index)
    {
        return index < rows_events.size() ? rows_events.at(index) : NULL;
    }

    void operator++()
    {
        it_++;
    }

    void operator--(int)
    {
        it_--;
    }
};


/**
 * @brief      Parse a rows event from binary stream
 *
 * @param      event  a binlog event struct
 *
 * @return     Pointer to the parsed rows event struct
 */
drizzle_binlog_rows_event_st *drizzle_binlog_parse_rows_event(
    drizzle_binlog_event_st *event);


drizzle_return_t drizzle_binlog_parse_row(
    drizzle_binlog_rows_event_st *event, unsigned char *ptr,
    unsigned char *columns_present);
