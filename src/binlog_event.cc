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

void dump_array_to_hex(const unsigned char *ptr, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        std::cout << std::hex << (uint32_t) ptr[i] << " ";
        if (i > 0 && i % 7 == 0 )
            std::cout << "\n";
    }
    std::cout << "\n";
}


struct xid_event_impl
{

  public :
    uint64_t _xid;
};

struct query_event_impl
{
    public :
    uint32_t _slave_proxy_id;
    uint32_t _execution_time;
    uint16_t _error_code;
    uint16_t _status_vars_length;
    unsigned char *_status_vars;
    unsigned char *_schema;
    unsigned char *_query;
};


struct tablemap_event_impl
{
    public :
        uint64_t _table_id;
        unsigned char* _flags;
        unsigned char* _schema_name;
        unsigned char* _table_name;
        uint64_t _column_count;
        unsigned char* _column_type_def;
        unsigned char* _field_metadata;
        unsigned char* _null_bitmap;
};

template<>
drizzle_binlog_query_event_st* drizzle_binlog_event_allocator::get()
{
    return &this->query_event;
}

template<>
drizzle_binlog_xid_event_st* drizzle_binlog_event_allocator::get()
{
    return &this->xid_event;
}

template<>
drizzle_binlog_tablemap_event_st* drizzle_binlog_event_allocator::get()
{
    return &this->tablemap_event;
}

drizzle_binlog_event_allocator::drizzle_binlog_event_allocator() {}

/*template<typename U, uint32_t V>
U drizzle_read_type(uint32_t *pos, unsigned char* data)
{
    static_assert(std::is_integral<U>::value,
        "The target type must integral");

    if ((sizeof(data)-*pos) - V < 0)
    {
        return numeric_limits<U>::max();
    }

    auto byte_size = sizeof(U);
    int i = 1;
    U value = ((U)data[*pos] & mask(byte_size*8));
    while (i < (int) byte_size)
    {
        value =((U)data[*pos+i]<<(i*8)|value);
        i++;
    }

    *pos += byte_size;

    return value;
}
*/

template<typename U, uint32_t V>
U drizzle_read_type(drizzle_binlog_event_st *binlog_event)
{
    static_assert(std::is_integral<U>::value,
        "The target type must integral");
    //U value = ((U)binlog_event->data_ptr[0]) & _mask;
    U value = mask(sizeof(U)*8);
    value =((U)binlog_event->data_ptr[0]) & value;
    uint64_t i = 1;
    while (i < V)
    {
        value = ((U)binlog_event->data_ptr[i] << (i*8) | value);
        i++;
    }

    binlog_event->data_ptr+=V;

    return value;
}

uint32_t ptr_dist(unsigned char* ptr1, unsigned char* ptr2)
{
  return ((const char*) ptr1 - (const char*) ptr2);
}

uint32_t drizzle_binlog_event_available_bytes(drizzle_binlog_event_st *event)
{
  return (event->length - ptr_dist(event->data_ptr, event->data));
}


template <typename U>
void drizzle_binlog_event_set_value(drizzle_binlog_event_st *binlog_event,
  U *value, uint32_t num_bytes)
{
    if (std::is_same<U, unsigned char*>::value)
    {
        *value = (U) realloc(*value, num_bytes + 1);
        memcpy(*value, binlog_event->data_ptr, num_bytes);
    }

    binlog_event->data_ptr += num_bytes;
}

uint64_t drizzle_binlog_get_encoded_len(drizzle_binlog_event_st * binlog_event)
{

    uint64_t len=0;

    if(drizzle_binlog_event_available_bytes(binlog_event)<1)
    {
        return 0;
    }

    uint32_t colBytes = bytes_col_count((uint32_t)binlog_event->data_ptr[0]);
    if(drizzle_binlog_event_available_bytes(binlog_event)-1<colBytes)
    {
        binlog_event->data_ptr++;
        return 0;
    }
    switch(colBytes)
    {
        case 1:
            len= (uint64_t)binlog_event->data_ptr[0];
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
    //pos+=colBytes + ((colBytes>1)? 1 : 0); // include first byte if colCount>1

    return len;
}

drizzle_binlog_xid_event_st* drizzle_binlog_get_xid_event( drizzle_binlog_event_st *event )
{
    dump_array_to_hex(drizzle_binlog_event_data(event),
        drizzle_binlog_event_length(event));
    auto xid_event = drizzle_binlog_event_allocator::instance().get<drizzle_binlog_xid_event_st>();
    ((xid_event_impl*) xid_event)->_xid = drizzle_read_type<uint64_t>(event);
    return xid_event;
}

drizzle_binlog_query_event_st *drizzle_binlog_get_query_event( drizzle_binlog_event_st *event )
{
    auto query_event = drizzle_binlog_event_allocator::instance().get<drizzle_binlog_query_event_st>();
    auto _impl = (query_event_impl*) query_event;
    _impl->_slave_proxy_id = drizzle_read_type<uint32_t>(event);
    _impl->_execution_time = drizzle_read_type<uint32_t>(event);
    uint16_t schema_length = drizzle_read_type<unsigned char>(event);
    _impl->_error_code = drizzle_read_type<uint16_t>(event);

    uint32_t status_vars_length = drizzle_read_type<uint16_t>(event);
    drizzle_binlog_event_set_value<unsigned char*>(
        event, &_impl->_status_vars, status_vars_length);
    drizzle_binlog_event_set_value<unsigned char*>(
        event, &_impl->_schema, schema_length);

    event->data_ptr++;
    drizzle_binlog_event_set_value<unsigned char*>(
        event, &_impl->_query, drizzle_binlog_event_available_bytes(event) - 4);

    return query_event;
}


drizzle_binlog_tablemap_event_st* drizzle_binlog_get_tablemap_event(drizzle_binlog_event_st *event)
{
    auto table_map_event = drizzle_binlog_event_allocator::instance().get<drizzle_binlog_tablemap_event_st>();
    auto _impl = (tablemap_event_impl*) table_map_event;

    _impl->_table_id = drizzle_read_type<uint64_t, 6>(event);

    drizzle_binlog_event_set_value<unsigned char*>(event, &_impl->_flags, 2);

    uint32_t len_enc = drizzle_read_type<uint32_t,1>(event);
    drizzle_binlog_event_set_value<unsigned char*>(event, &_impl->_schema_name, len_enc);
    event->data_ptr++;

    len_enc = drizzle_read_type<uint32_t,1>(event);
    drizzle_binlog_event_set_value<unsigned char*>(event, &_impl->_table_name, len_enc);
    event->data_ptr++;

    _impl->_column_count = drizzle_binlog_get_encoded_len(event);

    drizzle_binlog_event_set_value<unsigned char*>(event, &_impl->_column_type_def,
        _impl->_column_count);

    len_enc = drizzle_binlog_get_encoded_len(event);
    drizzle_binlog_event_set_value<unsigned char*>(event, &_impl->_field_metadata,
        len_enc);

    len_enc = (uint32_t) (_impl->_column_count + 7)/8;
    drizzle_binlog_event_set_value<unsigned char*>(event, &_impl->_null_bitmap,
        len_enc);

    return table_map_event;
}


drizzle_binlog_xid_event_st::drizzle_binlog_xid_event_st() : _impl(new xid_event_impl())
{}

drizzle_binlog_xid_event_st::~drizzle_binlog_xid_event_st()
{}

uint64_t drizzle_binlog_xid_event_st::xid()
{
    return _impl->_xid;
}


drizzle_binlog_query_event_st::drizzle_binlog_query_event_st() : _impl(new query_event_impl())
{}

drizzle_binlog_query_event_st::~drizzle_binlog_query_event_st() = default;

uint32_t  drizzle_binlog_query_event_st::slave_proxy_id()
{
    return _impl->_slave_proxy_id;
}

uint32_t drizzle_binlog_query_event_st::execution_time()
{
    return _impl->_execution_time;
}

uint16_t  drizzle_binlog_query_event_st::error_code()
{
    return _impl->_error_code;
}

uint16_t  drizzle_binlog_query_event_st::status_vars_length()
{
    return _impl->_status_vars_length;
}

unsigned char * drizzle_binlog_query_event_st::status_vars()
{
    return _impl->_status_vars;
}

unsigned char * drizzle_binlog_query_event_st::schema()
{
    return _impl->_schema;
}

unsigned char * drizzle_binlog_query_event_st::query()
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
    _impl(new tablemap_event_impl()) {}

/**
 * @brief      Destroys the object.
 */
drizzle_binlog_tablemap_event_st::~drizzle_binlog_tablemap_event_st() {}

uint64_t drizzle_binlog_tablemap_event_st::table_id()
{
    return _impl->_table_id;
}

unsigned char* drizzle_binlog_tablemap_event_st::flags()
{
    return _impl->_flags;
}

unsigned char* drizzle_binlog_tablemap_event_st::schema_name()
{
    return _impl->_schema_name;
}

unsigned char* drizzle_binlog_tablemap_event_st::table_name()
{
    return _impl->_table_name;
}

uint64_t drizzle_binlog_tablemap_event_st::column_count()
{
    return _impl->_column_count;
}

unsigned char* drizzle_binlog_tablemap_event_st::column_type_def()
{
    return _impl->_column_type_def;
}

unsigned char* drizzle_binlog_tablemap_event_st::field_metadata()
{
    return _impl->_field_metadata;
}

unsigned char* drizzle_binlog_tablemap_event_st::null_bitmap()
{
    return _impl->_null_bitmap;
}

