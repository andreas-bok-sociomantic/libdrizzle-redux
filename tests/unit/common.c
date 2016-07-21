/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab: 
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2013 Drizzle Developer Group
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

#include "common.h"
#include <yatl/lite.h>

drizzle_st *con= NULL;
drizzle_options_st *opts= NULL;

void close_connection_on_exit(void)
{
  if (con == NULL) {
    return;
  }
  
  drizzle_return_t ret= drizzle_quit(con);

  if (drizzle_options_get_non_blocking(opts))
  {
    ASSERT_EQ_(DRIZZLE_RETURN_IO_WAIT, ret, "drizzle_quit() : %s", drizzle_strerror(ret));
  }
  else
  {
    ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "drizzle_quit() : %s", drizzle_strerror(ret));
  }

  con= NULL;
  opts= NULL;
}

/* Common connection setup used by the unit tests. 
 * Connects to the server, deletes and recreates the libdrizzle schema. 
 */
void set_up_connection(void)
{
  set_up_connection_advanced(NULL, NULL, false);
}

/* Sets up a drizzle connection
 *
 * @param ev_watch_fn user specified callback function
 * @param ev_context  user specified context
 * @param non_block   flag for setting connection non-blocking
 */
void set_up_connection_advanced(drizzle_event_watch_fn *ev_watch_fn, void* ev_context, bool non_block)
{
  ASSERT_NULL_(con, "con opened twice?");

  opts = drizzle_options_create();
  if (non_block)
  {
    drizzle_options_set_non_blocking(opts, non_block);
  }
  ASSERT_NOT_NULL_(opts, "options cannot be NULL");

  // create
  con= drizzle_create(getenv("MYSQL_SERVER"),
                      getenv("MYSQL_PORT") ? atoi("MYSQL_PORT") : DRIZZLE_DEFAULT_TCP_PORT,
                      getenv("MYSQL_USER"),
                      getenv("MYSQL_PASSWORD"),
                      getenv("MYSQL_SCHEMA"), opts);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  if (ev_watch_fn != NULL)
  {
    drizzle_set_event_watch_fn(con, ev_watch_fn, ev_context);
  }

  // connect
  drizzle_return_t driz_ret= drizzle_connect(con);
  SKIP_IF_(driz_ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s", drizzle_strerror(driz_ret));
  atexit(close_connection_on_exit);
  if (drizzle_options_get_non_blocking(opts))
  {
    ASSERT_EQ_(DRIZZLE_RETURN_IO_WAIT, driz_ret, "%s(%s)", drizzle_error(con), drizzle_strerror(driz_ret));
  }
  else
  {
    ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "%s(%s)", drizzle_error(con), drizzle_strerror(driz_ret));
  }
}

void set_up_schema(const char *schema)
{
  drizzle_result_st VARIABLE_IS_NOT_USED *result;
  drizzle_return_t driz_ret;
  char sch_query[128];

  snprintf(sch_query, 127, "DROP SCHEMA IF EXISTS %s", schema);

  CHECKED_QUERY(sch_query);
  snprintf(sch_query, 127, "CREATE SCHEMA %s", schema);
  CHECKED_QUERY(sch_query);
  CHECK(drizzle_select_db(con, schema));
}

void tear_down_schema(const char *schema)
{
  drizzle_result_st VARIABLE_IS_NOT_USED *result;
  drizzle_return_t driz_ret;
  char sch_query[128];

  snprintf(sch_query, 127, "DROP SCHEMA IF EXISTS %s", schema);
  CHECKED_QUERY(sch_query);
}

