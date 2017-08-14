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
