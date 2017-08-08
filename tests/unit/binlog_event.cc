/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012-2013 Drizzle Developer Group
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

#include "tests/unit/common.h"

#include <cstdio>
#include <cstdlib>

void binlog_error(drizzle_return_t ret, drizzle_st *connection, void *context)
{
  (void)context;
  ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "%s(%s)", drizzle_error(connection),
             drizzle_strerror(ret));
}

void binlog_event(drizzle_binlog_event_st *event, void *context)
{
  (void)context;

  printf("callback\n");
  drizzle_binlog_event_types_t type = drizzle_binlog_event_type(event);
  printf("%s raw_len %d, event_len %d, timestamp: %d \n", drizzle_binlog_event_type_str(type),
    drizzle_binlog_event_raw_length(event),
    drizzle_binlog_event_length(event),
    drizzle_binlog_event_timestamp(event));

  if (type == DRIZZLE_EVENT_TYPE_XID)
  {
     drizzle_binlog_xid_event_st * xid_event = drizzle_binlog_get_xid_event(event);
     printf("xid %ld\n", xid_event->xid());
  }
  else if (type == DRIZZLE_EVENT_TYPE_TABLE_MAP)
  {

    drizzle_binlog_tablemap_event_st* table_map_event = drizzle_binlog_get_tablemap_event(event);
    printf("%ld", table_map_event->table_id());
  }
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_binlog_st *binlog;
  drizzle_return_t driz_ret;
  drizzle_result_st VARIABLE_IS_NOT_USED *result;

  set_up_connection();

  set_up_schema("test_binlog_event");

  CHECKED_QUERY("CREATE TABLE test_binlog_event.t1 "
    "(a int PRIMARY KEY auto_increment, b int)");

  CHECKED_QUERY("INSERT INTO test_binlog_event.t1 (b) "
    "VALUES (1),(2),(3)");

  char *binlog_file;
  uint32_t end_position;
  driz_ret = drizzle_binlog_get_filename(con, &binlog_file, &end_position, -1);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "Couldn't retrieve binlog filename: %s(%s)",
             drizzle_error(con), drizzle_strerror(driz_ret));

  printf("pos %d\n", end_position);
  binlog = drizzle_binlog_init(con, binlog_event, binlog_error, NULL, true);
  driz_ret = drizzle_binlog_start(binlog, 0, binlog_file, 0);

  SKIP_IF_(driz_ret == DRIZZLE_RETURN_ERROR_CODE, "Binlog is not open?: %s(%s)",
           drizzle_error(con), drizzle_strerror(driz_ret));
  ASSERT_EQ_(DRIZZLE_RETURN_EOF, driz_ret, "Drizzle binlog start failure: %s(%s)",
             drizzle_error(con), drizzle_strerror(driz_ret));

  CHECKED_QUERY("DROP TABLE test_binlog_event.t1");

  tear_down_schema("test_binlog_event");
  free(binlog_file);
  return EXIT_SUCCESS;
}