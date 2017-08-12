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

#include <memory>


/**
 * @brief      Check for type equality of primitive types or
 *             arrays of primitive types with rank 1
 *
 * @param      value  pointer to a value
 *
 * @tparam     V      Type to match
 * @tparam     U      Type of input
 *
 * @return     True if the types match, false otherwise
 */
template<typename V, typename U>
bool drizzle_check_type(U *value);

/**
 * @brief Read an integral type
 * @details [long description]
 *
 * @param pos index into the data array
 * @param data array of bytes
 * @tparam U the return type
 * @tparam V the number of bytes to read from the data
 * @return an integral of type U
 */
template<typename U, uint32_t V = sizeof(U)>
U drizzle_read_type(drizzle_binlog_event_st *binlog_event);

/**
 * @brief      Gets the bytes for a length-encoded numeric value from raw bytes
 *
 * @param      binlog_event  A pointer to a binlog event struct
 *
 * @return     the encoded length
 */
uint64_t drizzle_binlog_get_encoded_len(drizzle_binlog_event_st *binlog_event);


/**
 * @brief      Set a value on a binlog event struct using memcpy.
 *             Assumes that source value fits in the dest
 * @param      binlog_event  a pointer to a drizzle_binlog_event_st
 * @param      dest          pointer to the a member variable
 * @param[in]  num_bytes     number of bytes to copy from source to destination
 *
 * @tparam     U             The type of dest
 */
template <typename U>
void drizzle_binlog_event_set_value(drizzle_binlog_event_st *binlog_event,
                                    U *dest, uint32_t num_bytes = sizeof(U));

/**
 * @brief      Allocate memory before setting a value
 *
 * @param      binlog_event  a pointer to a drizzle_binlog_event_st
 * @param      dest          pointer to the a member variable
 * @param[in]  num_bytes     number of bytes to copy from source to destination
 *
 * @tparam     U             The type of dest
 */
template <typename U>
void drizzle_binlog_event_alloc_set_value(drizzle_binlog_event_st *binlog_event,
                                    U *dest, uint32_t num_bytes = sizeof(U));

/**
 * @brief      Get remaining bytes in the binlog event
 *
 * @param      event  a drizzle_binlog_event struct
 *
 * @return     number of available bytes
 */
uint32_t drizzle_binlog_event_available_bytes(drizzle_binlog_event_st *event);

/**
 * @brief      Gets the length encoded integer in bytes
 *
 * @param      __b   a char value
 *
 * @return     { description_of_the_return_value }
 */
#define bytes_col_count(__b) \
    ((uint64_t) (__b) < 0xfb ? 1 : \
     ((uint64_t) (__b) == 0xfc ? 2 : \
      ((uint64_t) (__b) == 0xfd ? 3 : 8)))

/**
 * @brief      Create a bitmask with all bits set to 1
 *
 * @param      __b   length of bitmask in bits
 *
 * @return     bitmask with a all bits set to 1
 */
#define mask(__b) \
    ((uint32_t) (__b) == 32 ? 0xffffffff : \
     ((uint32_t) (__b) == 24 ? 0xffffff : \
      ((uint32_t) (__b) == 16 ? 0xffff : \
       ((uint32_t) (__b) == 8 ? 0xff : 0xffffffffffffffff ))))

/**
 * @brief      Binlog Row based replication structure
 *
 * @description
 */
struct drizzle_binlog_rbr_st
{
    drizzle_binlog_xid_event_st *xid_event;
    drizzle_binlog_query_event_st *query_event;
    typedef std::unordered_map<const char*, drizzle_binlog_tablemap_event_st *> map_table_map_events;
    typedef map_table_map_events::iterator iterator_table_map_events;
    map_table_map_events table_map_events;
    typedef std::vector<drizzle_binlog_rows_event_st*> vec_rows_events;
    vec_rows_events rows_events;
    drizzle_binlog_rbr_fn *rbr_fn;
    void *context;
    drizzle_binlog_rbr_st();
    ~drizzle_binlog_rbr_st();

    /**
     * @brief      Adds a row based replication event to the structure
     *
     * @param      event  a drizzle_binlog_event structure
     */
    void add_event(drizzle_binlog_event_st *event);
};
