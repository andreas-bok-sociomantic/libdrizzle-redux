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


#pragma once

/**
 * @brief Read an integral type
 * @details [long description]
 *
 * @param pos index into the data array
 * @param data array of bytes
 * @tparam U the return type
 * @tparam V = the number of bytes to read from the data
 * @return an integral of type U
 */
template<typename U, uint32_t V = sizeof(U)>
U drizzle_read_type(uint32_t *pos, unsigned char* data);

template<typename U, uint32_t V = sizeof(U)>
U drizzle_read_type(drizzle_binlog_event_st* binlog_event);


/**
 * @brief Set a value on a binlog event struct
 *
 * @param binlog_event a pointer to a drizzle_binlog_event_st
 * @param dest pointer to the a member variable
 * @param num_bytes [description]
 */
template <typename U>
void drizzle_binlog_event_set_value(drizzle_binlog_event_st * binlog_event,
  U *dest, uint32_t num_bytes = sizeof(U));

#define bytes_col_count(__b) \
    ((uint64_t)(__b)<0xfb ? 1 : \
     ((uint64_t)(__b)==0xfc ? 2 : \
      ((uint64_t)(__b)==0xfd ? 3 : 8)))


#define mask(__b) \
    ((uint32_t)(__b)==32 ? 0xffffffff : \
     ((uint32_t)(__b)==24 ? 0xffffff : \
      ((uint32_t)(__b)==16 ? 0xffff : \
       ((uint32_t)(__b)==8 ? 0xff : 0xffffffffffffffff ))))


/**
 * \struct handles allocation of binlog event structs
 */
struct drizzle_binlog_event_allocator
{
  drizzle_binlog_query_event_st *query_event;
  drizzle_binlog_xid_event_st *xid_event;

  drizzle_binlog_event_allocator();

  ~drizzle_binlog_event_allocator()
  {
    free(this->query_event);
    free(this->xid_event);
  };

  template<typename U>
  U* get();

  static drizzle_binlog_event_allocator& instance()
  {
    static drizzle_binlog_event_allocator binlog_event_allocator;
    return binlog_event_allocator;
  }
};


// struct drizzle_binlog_xid_event_st
// {

//     uint64_t xid_;
//     drizzle_binlog_xid_event_st() : xid_(0)
//     {}

//     uint64_t xid();
// };

struct drizzle_binlog_query_event_st
{

  uint32_t slave_proxy_id;

  uint32_t execution_time;

  uint16_t error_code;

  uint16_t status_vars_length;

  unsigned char *status_vars;

  unsigned char *schema;

  unsigned char *query;
};


// struct drizzle_binlog_rows_event_st
// {
//     void parse_postheader(unsigned char *_data) {
//       this->data = _data;
//     };

//     void parse_payload() {};
// };


// struct drizzle_binlog_tablemap_event_st
// {
//   void parse_postheader(unsigned char *_data) {
//     this->data = _data;
//   };

//     void parse_payload() {};
// };
