/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Drizzle Client & Protocol Library
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
 *
 */

#include <libdrizzle-redux/libdrizzle.h>
#include <yatl/lite.h>


int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    drizzle_binlog_event_types_t event_types[14];
    memcpy(
        event_types,
        (drizzle_binlog_event_types_t[])
        {
            DRIZZLE_EVENT_TYPE_FORMAT_DESCRIPTION,
            DRIZZLE_EVENT_TYPE_ROWS_QUERY,
            DRIZZLE_EVENT_TYPE_V2_WRITE_ROWS,
            DRIZZLE_EVENT_TYPE_OBSOLETE_UPDATE_ROWS,
            DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS,
            DRIZZLE_EVENT_TYPE_XID,
            DRIZZLE_EVENT_TYPE_V2_DELETE_ROWS,
            DRIZZLE_EVENT_TYPE_V1_UPDATE_ROWS,
            DRIZZLE_EVENT_TYPE_QUERY,
            DRIZZLE_EVENT_TYPE_OBSOLETE_WRITE_ROWS,
            DRIZZLE_EVENT_TYPE_TABLE_MAP,
            DRIZZLE_EVENT_TYPE_OBSOLETE_DELETE_ROWS,
            DRIZZLE_EVENT_TYPE_V1_WRITE_ROWS,
            DRIZZLE_EVENT_TYPE_V1_DELETE_ROWS
        },
    sizeof(event_types));

    // 4 byte array where a bit encodes whether an event type is a rows event
    // The variable `ptr` points to the second byte.
    //          |               |
    // 00101011 11010100 00110000 01001001
    unsigned char bytes[] = { 0x2B, 0xD4, 0x30, 0x49 };
    unsigned char *ptr = &bytes[1];

    // iterate the array of event types of validate the API functions:
    //   - drizzle_binlog_is_rows_event
    //   - bit_is_set
    for (uint i = 0; i < 14; i++)
    {
        ASSERT_EQ_(drizzle_binlog_is_rows_event(event_types[i]),
            bit_is_set(ptr, i),
            "Expected %d %s to be a rows event type",
            i, drizzle_binlog_event_type_str(event_types[i]));
    }

    return EXIT_SUCCESS;
}
