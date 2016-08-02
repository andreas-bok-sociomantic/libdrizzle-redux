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

#include <libdrizzle-5.1/libdrizzle.h>

#include "tests/unit/common.h"

#include <cstdio>
#include <cstdlib>

/* user provided callback
*/
static drizzle_return_t con_event_watch_fn(drizzle_st *_con, short _events, void *_context)
{
  fprintf(stdout, BLU "UTEST: " RESET "con_event_watch_fn status %d, events %d, cxt %d, "
    "error_code=%d\n",
  drizzle_status(_con), _events, _context == NULL, drizzle_error_code(_con));

  short events = 0;

  events |= _events;
  drizzle_set_events(con, events);
  return DRIZZLE_RETURN_OK;
}

void binlog_error(drizzle_return_t ret, drizzle_st *connection, void *context)
{
  fprintf(stderr, BLU "UTEST: " RESET "binlog_error %s\n", drizzle_strerror(ret));
  (void) context;
  ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "%s(%s)", drizzle_error(connection), drizzle_strerror(ret));
}

void binlog_event(drizzle_binlog_event_st *event, void *context)
{
  (void) context;
  uint32_t timestamp;
  drizzle_st *_con = (drizzle_st *) context;
  timestamp= drizzle_binlog_event_timestamp(event);

  fprintf(stderr, "UTEST: binlog_event %d con.status %d\n",  drizzle_binlog_event_type(event),
    drizzle_status(_con));

  /* Test to see if timestamp is greater than 2012-01-01 00:00:00, corrupted
   * timestamps will have weird values that shoud fail this after several
   * events.  Also rotate event doesn't have a timestamp so need to add 0
   * to this test */
  ASSERT_FALSE_(((timestamp < 1325376000) && (timestamp != 0)), "Bad timestamp retrieved: %u", timestamp);

  /* An event higher than the max known is bad, either we don't know about
   * new events or type is corrupted */
  ASSERT_FALSE_((drizzle_binlog_event_type(event) >= DRIZZLE_EVENT_TYPE_END), "Bad event type: %d", drizzle_binlog_event_type(event));
}


/*Validate connecting to and parsing binlog*/
void validate_binlog()
{
  drizzle_binlog_st *binlog;
  drizzle_return_t ret;
  int _server_id = 0; // read binary logs to end and finish
  bool _non_blocking = true;
  short poll_events = POLLIN;
  poll_events |= POLLOUT;
  set_up_connection_advanced(con_event_watch_fn, (void*) NULL, _non_blocking);

  drizzle_set_verbose(con, DRIZZLE_VERBOSE_DEBUG);

  //drizzle_set_events(con, poll_events);

  binlog= drizzle_binlog_init(con, binlog_event, binlog_error, (void*) con, true);

  ret= drizzle_binlog_start2(binlog, _server_id, "mysql-bin.000038", 0);

  SKIP_IF_(ret == DRIZZLE_RETURN_ERROR_CODE, "Binlog is not open?: %s(%s)",
    drizzle_error(con), drizzle_strerror(ret));

  //ASSERT_EQ_(DRIZZLE_BINLOG_STATE_PREPARE_COMPLETE, drizzle_binlog_get_state(binlog),
  //  "Binlog not started correctly");
  fprintf(stderr, "UTEST: ret %s\n", drizzle_strerror(ret));
  //ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "Drizzle binlog start failure: %s(%s)", drizzle_error(con), drizzle_strerror(ret));
}

/* Try to do the hack for setting master_binlog_checksum with
*/
void validate_checksum()
{
  drizzle_binlog_st *binlog;
  drizzle_return_t ret;
  bool _non_blocking = true;

  short poll_events = POLLIN;

  set_up_connection_advanced(con_event_watch_fn, (void*) NULL, _non_blocking);

  drizzle_set_verbose(con, DRIZZLE_VERBOSE_INFO);

  drizzle_set_events(con, poll_events);

  binlog= drizzle_binlog_init(con, binlog_event, binlog_error, (void*) con, true);

  ret = drizzle_binlog_set_checksum(binlog);
  ASSERT_EQ_(DRIZZLE_BINLOG_STATE_SET_CHECKSUM_COMPLETE, drizzle_binlog_get_state(binlog),
    "Binlog checksum not set");

  fprintf(stderr, "UTEST: drizzle_wait: ret %s\n", drizzle_strerror(ret));
}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  validate_binlog();
//  validate_checksum();

  return EXIT_SUCCESS;
}
