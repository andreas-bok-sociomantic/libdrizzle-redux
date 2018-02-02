/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (c) 2018 sociomantic labs GmbH
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

#include <yatl/lite.h>
#include <libdrizzle-redux/libdrizzle.h>
#include "tests/unit/common.h"

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_result_st *result;
  drizzle_row_t row;
  drizzle_return_t driz_ret;

  set_up_connection();

  // Check MySQL server version string
  result = drizzle_query(con, "SHOW variables like 'version'", 0, &driz_ret);

  CHECK(drizzle_result_buffer(result));
  ASSERT_EQ(1, drizzle_result_row_count(result));
  row = drizzle_row_next(result);
  ASSERT_STREQ(row[1], drizzle_server_version(con));

  // Ping server
  result = drizzle_ping(con, &driz_ret);
  drizzle_result_buffer(result);
  ASSERT_EQ(DRIZZLE_RETURN_OK, driz_ret);

  // Check file descriptor for the connection
  ASSERT_NEQ(-1, drizzle_fd(con));

  // Check server charset
  ASSERT_NEQ(DRIZZLE_CHARSET_NONE, drizzle_charset(con));

  // Connection status
  drizzle_status(con);

  // Check max allowed packet size: <= 0xFFFFFFFF
  ASSERT_TRUE(drizzle_max_packet_size(con) <= UINT32_MAX);

  const unsigned char *scramble = drizzle_scramble(con);
  ASSERT_NOT_NULL(scramble);
}
