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

using namespace std;

template<>
drizzle_binlog_query_event_st* drizzle_binlog_event_allocator::get()
{
    return this->query_event;
}

template<>
drizzle_binlog_xid_event_st* drizzle_binlog_event_allocator::get()
{
    return this->xid_event;
}


drizzle_binlog_event_allocator::drizzle_binlog_event_allocator()
{
    this->xid_event = new (std::nothrow) drizzle_binlog_xid_event_st;
    this->query_event = new (std::nothrow) drizzle_binlog_query_event_st;
}

template<typename U, uint32_t V>
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


template<typename U, uint32_t V>
U drizzle_read_type(drizzle_binlog_event_st *binlog_event)
{
    static_assert(std::is_integral<U>::value,
        "The target type must integral");
    auto byte_size = sizeof(U);
    U value = ((U)binlog_event->data_ptr) & mask(byte_size*8);
    uint64_t i = 1;
    while (i < byte_size)
    {
        value = ((U)(binlog_event->data_ptr) + i) << (i * 8)| value;
        i++;
    }

    return value;
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


drizzle_binlog_xid_event_st* drizzle_binlog_get_xid_event( drizzle_binlog_event_st *event )
{
    auto xid_event = drizzle_binlog_event_allocator::instance().get<drizzle_binlog_xid_event_st>();

    xid_event->xid_ = drizzle_read_type<uint64_t>(event);
    return xid_event;
}

drizzle_binlog_query_event_st *drizzle_binlog_get_query_event( drizzle_binlog_event_st *event )
{
    auto query_event = drizzle_binlog_event_allocator::instance().get<drizzle_binlog_query_event_st>();

    // query_event->slave_proxy_id = drizzle_read_type<uint32_t>(&query_event->data_index,
    //     event->data);

    // query_event->execution_time = drizzle_read_type<uint32_t>(&query_event->data_index,
    //     event->data);


    // uint16_t schema_length = drizzle_read_type<unsigned char>(&query_event->data_index,
    //     event->data);


    // query_event->error_code = drizzle_read_type<uint16_t>(&query_event->data_index,
    //     event->data);

    // uint32_t status_vars_length = drizzle_read_type<uint16_t>(&query_event->data_index,
    //     event->data);

    uint16_t schema_length = 10;
    uint32_t status_vars_length = 19;
    drizzle_binlog_event_set_value<unsigned char*>(
        event, &query_event->status_vars, status_vars_length);

    drizzle_binlog_event_set_value<unsigned char*>(
        event, &query_event->schema, schema_length);

    event->data_ptr++;

    drizzle_binlog_event_set_value<unsigned char*>(
        event, &query_event->query,
        event->length -
        ((const char*) event->data_ptr - (const char*) event->data) - 4);

    return query_event;
}


