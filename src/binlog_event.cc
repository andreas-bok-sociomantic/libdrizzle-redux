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

struct drizzle_binlog_event_object_pool_st
{
  drizzle_binlog_xid_event_st *xid_event;
  drizzle_binlog_query_event_st *query_event;
  drizzle_binlog_tablemap_event_st *table_map_event;
  drizzle_binlog_event_object_pool_st()
  {
    this->xid_event = new drizzle_binlog_xid_event_st();
    this->query_event = new drizzle_binlog_query_event_st();
    this->table_map_event = new drizzle_binlog_tablemap_event_st();
  }
};

static drizzle_binlog_event_object_pool_st  *drizzle_binlog_event_object_pool =
    new drizzle_binlog_event_object_pool_st();

struct drizzle_binlog_event_header_st::binlog_event_header_impl
{
    uint32_t _timestamp;
    drizzle_binlog_event_types_t _type;
    uint32_t _server_id;
    uint32_t _length;
    uint32_t _next_pos;
    uint16_t _header_flags;
    uint16_t _extra_flags;
    uint32_t _checksum;
    binlog_event_header_impl(drizzle_binlog_event_st *event=NULL)
    {
        if (event !=NULL)
        {
            this->_timestamp = event->timestamp;
            this->_type = event->type;
            this->_server_id = event->server_id;
            this->_length = event->length;
            this->_next_pos = event->next_pos;
            this->_header_flags = event->flags;
            this->_extra_flags = event->extra_flags;
            this->_checksum = event->checksum;
        }
    }
};

void drizzle_binlog_event_header_st::set_event_header(
    drizzle_binlog_event_st *event)
{
    _impl->_timestamp = event->timestamp;
    _impl->_type = event->type;
    _impl->_server_id = event->server_id;
    _impl->_length = event->length;
    _impl->_next_pos = event->next_pos;
    _impl->_header_flags = event->flags;
    _impl->_extra_flags = event->extra_flags;
    _impl->_checksum = event->checksum;
}

uint32_t drizzle_binlog_event_header_st::timestamp()
{
    return _impl->_timestamp;
}

drizzle_binlog_event_types_t drizzle_binlog_event_header_st::type()
{
    return _impl->_type;
}

uint32_t drizzle_binlog_event_header_st::server_id()
{
    return _impl->_server_id;
}

uint32_t drizzle_binlog_event_header_st::length()
{
    return _impl->_length;
}

uint32_t drizzle_binlog_event_header_st::next_pos()
{
    return _impl->_next_pos;
}

uint16_t drizzle_binlog_event_header_st::header_flags()
{
    return _impl->_header_flags;
}

uint16_t drizzle_binlog_event_header_st::extra_flags()
{
    return _impl->_extra_flags;
}

uint32_t drizzle_binlog_event_header_st::checksum()
{
    return _impl->_checksum;
}

drizzle_binlog_event_header_st::drizzle_binlog_event_header_st(drizzle_binlog_event_st *event) :
    _impl(new binlog_event_header_impl(event))
{
}

drizzle_binlog_event_header_st::~drizzle_binlog_event_header_st() = default;

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
    uint8_t *_status_vars;
    unsigned char _schema[DRIZZLE_MAX_DB_SIZE];
    unsigned char *_query;
};

struct drizzle_binlog_rows_event_st::rows_event_impl
{
public:
    uint64_t _table_id;
    uint64_t _column_count;
    uint32_t _field_metadata_len;
    uint8_t *column_type_def;
    uint8_t *field_metadata;
    uint64_t bitmap_size;
};


struct drizzle_binlog_tablemap_event_st::tablemap_event_impl
{
public:
    uint64_t _table_id;
    uint8_t _flags[2];
    unsigned char _schema_name[DRIZZLE_MAX_DB_SIZE];
    unsigned char _table_name[DRIZZLE_MAX_TABLE_SIZE];
    uint64_t _column_count;
    uint8_t *_column_type_def;
    uint8_t *_field_metadata;
    uint64_t _field_metadata_len;
    uint8_t *_null_bitmap;
};

template<typename V, typename U>
bool drizzle_check_type(U *value)
{
    if (std::rank<U>::value == 0)
    {
        typedef typename std::remove_cv<U>::type TYPE0;
        typedef typename std::remove_pointer<TYPE0>::type _TYPE0;
        return std::is_same<_TYPE0, V>::value;
    }
    else if (std::rank<U>::value == 1 )
    {
        typedef typename std::remove_cv<typeof((*value)[0])>::type TYPE1;
        typedef typename std::remove_pointer<TYPE1>::type _TYPE1;
        return std::is_same<_TYPE1, V>::value;
    }
    return false;
}


template<typename U, uint32_t V>
U drizzle_read_type(drizzle_binlog_event_st *binlog_event)
{
    static_assert(std::is_integral<U>::value,
                  "The target type must an integral");
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
                                    U *dest, uint32_t num_bytes)
{
    if (drizzle_binlog_event_available_bytes(binlog_event) < num_bytes)
    {
        printf("Insufficient data (%d) to read %d bytes\n",
               drizzle_binlog_event_available_bytes(binlog_event),
               num_bytes);
        assert(false);
    }

    if (drizzle_check_type<char, U>(dest) ||
        drizzle_check_type<unsigned char, U>(dest))
    {
        memcpy(*dest, binlog_event->data_ptr, num_bytes);
    }
    else if (drizzle_check_type<uint8_t, U>(dest))
    {
        printf("copying uint8_t\n");
        memcpy(*dest, binlog_event->data_ptr, num_bytes);
    }

    binlog_event->data_ptr += num_bytes;
}

template <typename U>
void drizzle_binlog_event_alloc_set_value(drizzle_binlog_event_st *binlog_event,
                                          U *value, uint32_t num_bytes)
{
    *value = (U) realloc(*value, num_bytes + 1);
    drizzle_binlog_event_set_value<U>(binlog_event, value, num_bytes);
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
    auto *xid_event = drizzle_binlog_event_object_pool->xid_event;
    xid_event->set_event_header(event);
    xid_event->parse(event);
    return xid_event;
}

drizzle_binlog_query_event_st *drizzle_binlog_get_query_event(
    drizzle_binlog_event_st *event)
{
    auto query_event = drizzle_binlog_event_object_pool->query_event;
    query_event->set_event_header(event);
    query_event->parse(event);
    return query_event;
}


drizzle_binlog_tablemap_event_st *drizzle_binlog_get_tablemap_event(
    drizzle_binlog_event_st *event)
{

    auto table_map_event = drizzle_binlog_event_object_pool->table_map_event;
    table_map_event->set_event_header(event);
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

drizzle_binlog_xid_event_st::drizzle_binlog_xid_event_st(drizzle_binlog_event_st *event) :
    drizzle_binlog_event_header_st(event), _impl(new xid_event_impl())
{}

drizzle_binlog_xid_event_st::~drizzle_binlog_xid_event_st() = default;

void drizzle_binlog_xid_event_st::parse(drizzle_binlog_event_st *event)
{
    _impl->_xid = drizzle_read_type<uint64_t>(event);
}

uint64_t drizzle_binlog_xid_event_st::xid()
{
    return _impl->_xid;
}

std::ostream &operator<<(std::ostream &_stream, drizzle_binlog_xid_event_st &e)
{
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
    uint16_t schema_length = drizzle_read_type<uint8_t>(event);
    _impl->_error_code = drizzle_read_type<uint16_t>(event);

    uint32_t value_len = drizzle_read_type<uint16_t>(event);
    drizzle_binlog_event_alloc_set_value<uint8_t*>(event, &_impl->_status_vars,
                                         value_len);
    drizzle_binlog_event_set_value(event, &_impl->_schema, schema_length);
    _impl->_schema[schema_length] = '\0';

    event->data_ptr++;
    value_len = drizzle_binlog_event_available_bytes(event) - 4;
    drizzle_binlog_event_alloc_set_value(event, &_impl->_query, value_len);
    _impl->_query[value_len] = '\0';
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

uint8_t *drizzle_binlog_query_event_st::status_vars()
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

    drizzle_binlog_event_set_value(event, &_impl->_flags, 2);

    uint32_t len_enc = drizzle_read_type<uint32_t, 1>(event);
    drizzle_binlog_event_set_value(event, &_impl->_schema_name, len_enc);
    _impl->_schema_name[len_enc] = '\0';
    event->data_ptr++;

    len_enc = drizzle_read_type<uint32_t, 1>(event);
    drizzle_binlog_event_set_value(event, &_impl->_table_name, len_enc);
    _impl->_table_name[len_enc] = '\0';
    event->data_ptr++;

    _impl->_column_count = drizzle_binlog_get_encoded_len(event);

    drizzle_binlog_event_alloc_set_value(event,
                                         &_impl->_column_type_def,
                                         _impl->_column_count);

    _impl->_field_metadata_len = drizzle_binlog_get_encoded_len(event);
    drizzle_binlog_event_alloc_set_value(event,
                                         &_impl->_field_metadata,
                                         _impl->_field_metadata_len);

    len_enc = (uint32_t) (_impl->_column_count + 7) / 8;
    drizzle_binlog_event_alloc_set_value(event, &_impl->_null_bitmap,
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

uint8_t *drizzle_binlog_tablemap_event_st::column_type_def()
{
    return _impl->_column_type_def;
}

uint8_t *drizzle_binlog_tablemap_event_st::field_metadata()
{
    return _impl->_field_metadata;
}

uint64_t drizzle_binlog_tablemap_event_st::field_metadata_len()
{
    return _impl->_field_metadata_len;
}

uint8_t *drizzle_binlog_tablemap_event_st::null_bitmap()
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
    drizzle_binlog_event_set_value(event, &_impl->column_type_def,
        table_map_event->column_count());

    // metadata definition
    drizzle_binlog_event_set_value(event, &_impl->field_metadata,
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
            drizzle_binlog_event_alloc_set_value(event,
                                                 &buffer,
                                                 len - 2);
        }
    }

    // column_count
    _impl->_column_count = drizzle_binlog_get_encoded_len(event);

    _impl->bitmap_size = (_impl->_column_count + 7)/8;

} // drizzle_binlog_rows_event_st::parse
