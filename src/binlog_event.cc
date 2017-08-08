/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2015 Andreas Bok Andersen (andreas.bok@sociomantic.com)
 * All rights reserved.
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
 *
 */

#include "config.h"
#include "src/common.h"

#include <limits>
#include <cstdint>
#include <type_traits>
#include <inttypes.h>
#include <string>
#include <iostream>

using namespace std;

struct drizzle_binlog_xid_event_st::xid_event_impl
{
public:
    uint64_t _xid;
};

struct drizzle_binlog_query_event_st::query_event_impl
{
public:
    uint32_t _slave_proxy_id;
    uint32_t _execution_time;
    uint16_t _error_code;
    uint16_t _status_vars_length;
    unsigned char *_status_vars;
    unsigned char *_schema;
    unsigned char *_query;
};

struct drizzle_binlog_rows_event_st::rows_event_impl
{
public:
    uint64_t _table_id;
    uint64_t _column_count;
    uint32_t _field_metadata_len;
    unsigned char *column_type_def;
    unsigned char *field_metadata;
};


struct drizzle_binlog_tablemap_event_st::tablemap_event_impl
{
public:
    uint64_t _table_id;
    unsigned char *_flags;
    unsigned char *_schema_name;
    unsigned char *_table_name;
    uint64_t _column_count;
    unsigned char *_column_type_def;
    unsigned char *_field_metadata;
    uint64_t _field_metadata_len;
    unsigned char *_null_bitmap;
};

template<typename U, uint32_t V>
U drizzle_read_type(drizzle_binlog_event_st *binlog_event)
{
    static_assert(std::is_integral<U>::value,
                  "The target type must integral");
    // U value = ((U)binlog_event->data_ptr[0]) & _mask;
    U value = mask(sizeof(U) * 8);
    value = ((U) binlog_event->data_ptr[0]) & value;
    uint64_t i = 1;
    while (i < V)
    {
        value = ((U) binlog_event->data_ptr[i] << (i * 8) | value);
        i++;
    }

    binlog_event->data_ptr += V;

    return value;
}

uint32_t drizzle_binlog_event_available_bytes(drizzle_binlog_event_st *event)
{
    return (event->length - ptr_dist(event->data_ptr, event->data));
}


template <typename U>
void drizzle_binlog_event_set_value(drizzle_binlog_event_st *binlog_event,
                                    U *value, uint32_t num_bytes)
{

    if (drizzle_binlog_event_available_bytes(binlog_event) < num_bytes)
    {
        printf("Insufficient data (%d) to read %d bytes\n",
            drizzle_binlog_event_available_bytes(binlog_event),
            num_bytes);
        assert(false);
    }

    if (std::is_same<U, unsigned char *>::value)
    {
        *value = (U) realloc(*value, num_bytes + 1);
        memcpy(*value, binlog_event->data_ptr, num_bytes);
    }

    binlog_event->data_ptr += num_bytes;
}

uint64_t drizzle_binlog_get_encoded_len(drizzle_binlog_event_st *binlog_event)
{

    uint64_t len = 0;

    if (drizzle_binlog_event_available_bytes(binlog_event) < 1)
    {
        return 0;
    }

    uint32_t colBytes = bytes_col_count((uint32_t) binlog_event->data_ptr[0]);
    if (drizzle_binlog_event_available_bytes(binlog_event) - 1 < colBytes)
    {
        binlog_event->data_ptr++;
        return 0;
    }
    switch (colBytes)
    {
        case 1:
            len = (uint64_t) binlog_event->data_ptr[0];
            binlog_event->data_ptr++;
            break;
        case 2:
            binlog_event->data_ptr++;
            len = drizzle_read_type<ushort>(binlog_event);
            break;
        case 3:
            binlog_event->data_ptr++;
            len = drizzle_read_type<uint32_t, 3>(binlog_event);
            break;
        case 8:
            binlog_event->data_ptr++;
            len = drizzle_read_type<uint64_t>(binlog_event);
            break;
        default:
            break;
    }

    return len;
} // drizzle_binlog_get_encoded_len

drizzle_binlog_xid_event_st *drizzle_binlog_get_xid_event(
    drizzle_binlog_event_st *event)
{
    dump_array_to_hex(drizzle_binlog_event_data(event),
                      drizzle_binlog_event_length(event));
    drizzle_binlog_xid_event_st *xid_event = new drizzle_binlog_xid_event_st();

    xid_event->parse(event);
    return xid_event;
}

drizzle_binlog_query_event_st *drizzle_binlog_get_query_event(
    drizzle_binlog_event_st *event)
{
    auto query_event = new drizzle_binlog_query_event_st();

    query_event->parse(event);
    return query_event;
}


drizzle_binlog_tablemap_event_st *drizzle_binlog_get_tablemap_event(
    drizzle_binlog_event_st *event)
{

    auto table_map_event = new drizzle_binlog_tablemap_event_st();

    table_map_event->parse(event);
    return table_map_event;
}

drizzle_binlog_rows_event_st *drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event,
    drizzle_binlog_tablemap_event_st *table_map_event)
{
    auto rows_event = new drizzle_binlog_rows_event_st();

    rows_event->parse(event, table_map_event);
    return rows_event;
}

drizzle_binlog_xid_event_st::drizzle_binlog_xid_event_st() : _impl(
        new xid_event_impl())
{
}

drizzle_binlog_xid_event_st::~drizzle_binlog_xid_event_st()
{
}

void drizzle_binlog_xid_event_st::parse(drizzle_binlog_event_st *event)
{
    _impl->_xid = drizzle_read_type<uint64_t>(event);
}

uint64_t drizzle_binlog_xid_event_st::xid()
{
    return _impl->_xid;
}

std::ostream &operator<<(std::ostream & _stream, drizzle_binlog_xid_event_st &e) {
    _stream << "Xid Event( xid=" << e.xid() << ")\n";
    return _stream;
}


drizzle_binlog_query_event_st::drizzle_binlog_query_event_st() : _impl(
        new query_event_impl())
{
}

drizzle_binlog_query_event_st::~drizzle_binlog_query_event_st() = default;

void drizzle_binlog_query_event_st::parse(drizzle_binlog_event_st *event)
{
    _impl->_slave_proxy_id = drizzle_read_type<uint32_t>(event);
    _impl->_execution_time = drizzle_read_type<uint32_t>(event);
    uint16_t schema_length = drizzle_read_type<unsigned char>(event);
    _impl->_error_code = drizzle_read_type<uint16_t>(event);

    uint32_t status_vars_length = drizzle_read_type<uint16_t>(event);
    drizzle_binlog_event_set_value<unsigned char *>(
        event, &_impl->_status_vars, status_vars_length);
    drizzle_binlog_event_set_value<unsigned char *>(
        event, &_impl->_schema, schema_length);

    event->data_ptr++;
    drizzle_binlog_event_set_value<unsigned char *>(
        event, &_impl->_query, drizzle_binlog_event_available_bytes(event) - 4);
}

uint32_t drizzle_binlog_query_event_st::slave_proxy_id()
{
    return _impl->_slave_proxy_id;
}

uint32_t drizzle_binlog_query_event_st::execution_time()
{
    return _impl->_execution_time;
}

uint16_t drizzle_binlog_query_event_st::error_code()
{
    return _impl->_error_code;
}

uint16_t drizzle_binlog_query_event_st::status_vars_length()
{
    return _impl->_status_vars_length;
}

unsigned char *drizzle_binlog_query_event_st::status_vars()
{
    return _impl->_status_vars;
}

unsigned char *drizzle_binlog_query_event_st::schema()
{
    return _impl->_schema;
}

unsigned char *drizzle_binlog_query_event_st::query()
{
    return _impl->_query;
}


/**
 * drizzle_binlog_tablemap_event_st API implementation
 */

/**
 * @brief      Constructs the object.
 */
drizzle_binlog_tablemap_event_st::drizzle_binlog_tablemap_event_st() :
    _impl(new tablemap_event_impl())
{
}

/**
 * @brief      Destroys the object.
 */
drizzle_binlog_tablemap_event_st::~drizzle_binlog_tablemap_event_st() = default;


void drizzle_binlog_tablemap_event_st::parse(drizzle_binlog_event_st *event)
{
    _impl->_table_id = drizzle_read_type<uint64_t, 6>(event);

    drizzle_binlog_event_set_value<unsigned char *>(event, &_impl->_flags, 2);

    uint32_t len_enc = drizzle_read_type<uint32_t, 1>(event);
    drizzle_binlog_event_set_value<unsigned char *>(event, &_impl->_schema_name,
                                                    len_enc);
    event->data_ptr++;

    len_enc = drizzle_read_type<uint32_t, 1>(event);
    drizzle_binlog_event_set_value<unsigned char *>(event, &_impl->_table_name,
                                                    len_enc);
    event->data_ptr++;

    _impl->_column_count = drizzle_binlog_get_encoded_len(event);

    drizzle_binlog_event_set_value<unsigned char *>(event,
                                                    &_impl->_column_type_def,
                                                    _impl->_column_count);

    // len_enc = drizzle_binlog_get_encoded_len(event);
    _impl->_field_metadata_len = drizzle_binlog_get_encoded_len(event);
    drizzle_binlog_event_set_value<unsigned char *>(event,
                                                    &_impl->_field_metadata,
                                                    _impl->_field_metadata_len);

    len_enc = (uint32_t) (_impl->_column_count + 7) / 8;
    drizzle_binlog_event_set_value<unsigned char *>(event, &_impl->_null_bitmap,
                                                    len_enc);

} // drizzle_binlog_tablemap_event_st::parse

uint64_t drizzle_binlog_tablemap_event_st::table_id()
{
    return _impl->_table_id;
}

unsigned char *drizzle_binlog_tablemap_event_st::schema_name()
{
    return _impl->_schema_name;
}

unsigned char *drizzle_binlog_tablemap_event_st::table_name()
{
    return _impl->_table_name;
}

uint64_t drizzle_binlog_tablemap_event_st::column_count()
{
    return _impl->_column_count;
}

unsigned char *drizzle_binlog_tablemap_event_st::column_type_def()
{
    return _impl->_column_type_def;
}

unsigned char *drizzle_binlog_tablemap_event_st::field_metadata()
{
    return _impl->_field_metadata;
}

uint64_t drizzle_binlog_tablemap_event_st::field_metadata_len()
{
    return _impl->_field_metadata_len;
}

unsigned char *drizzle_binlog_tablemap_event_st::null_bitmap()
{
    return _impl->_null_bitmap;
}


drizzle_binlog_rows_event_st::drizzle_binlog_rows_event_st() :
    _impl(new rows_event_impl())
{

}
drizzle_binlog_rows_event_st::~drizzle_binlog_rows_event_st() = default;

void drizzle_binlog_rows_event_st::parse(drizzle_binlog_event_st *event,
                                         drizzle_binlog_tablemap_event_st *table_map_event)
{

    // column definition
    _impl->column_type_def = (unsigned char *) realloc(_impl->column_type_def,
                                                       table_map_event->column_count());
    memcpy(_impl->column_type_def, table_map_event->column_type_def(),
           table_map_event->column_count());

    // metadata definition
    _impl->field_metadata = (unsigned char *) realloc(_impl->field_metadata,
                                                      table_map_event->field_metadata_len());
    memcpy(_impl->field_metadata, table_map_event->field_metadata(),
           table_map_event->field_metadata_len());

    _impl->_table_id = drizzle_read_type<uint64_t, 6>(event);

    uint16_t flags = drizzle_read_type<uint16_t>(event);

    if ( _impl->_table_id == 0x00ffffff && flags == (1 << 0))
    {
        return;
    }

    unsigned char *buffer;
    if (drizzle_binlog_rows_event_version(event->type) == 2)
    {
        auto len = drizzle_read_type<uint16_t>(event);
        if ( len > 2 )
        {
            drizzle_binlog_event_set_value<unsigned char *>(event, &buffer,
                                                            len - 2);
        }
    }

    // column_count
    _impl->_column_count = drizzle_binlog_get_encoded_len(event);

    // auto bitmap_size = (_impl->_column_count + 7)/8;
} // drizzle_binlog_rows_event_st::parse
