/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2018 Sociomantic Tsunami
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

/**
 *  Test encrypted connections to a MySQL server
 *  The key, certificate and certificate authority must have the default MySQL
 *  SSL filenames
 *   - server-key.pem
 *   - server-cert.pem
 *   - ca.pem
 *
 *  The path to the certificate and key files must be specified with the
 *  environment variable DRIZZLE_MYSQL_CA_PATH
 */
int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  drizzle_return_t driz_ret;

  set_up_connection();

  // Check environment variable DRIZZLE_MYSQL_CA_PATH is set
  const char * ca_path = getenv("DRIZZLE_MYSQL_CA_PATH");
  if (ca_path == NULL)
  {
    SKIP_(true, "DRIZZLE_MYSQL_CA_PATH (path to certificate and key files) not set");
  }

  // Check if server has SSL capabilities enabled.
  if ((drizzle_capabilities(con) & DRIZZLE_CAPABILITIES_SSL) == 0)
  {
    SKIP_(true, "SSL not enabled for connection to MySQL server. Skipping test");
  }

  drizzle_close(con);

  drizzle_row_t row;
  drizzle_result_st VARIABLE_IS_NOT_USED *result;
  char ssl_key[256];
  char ssl_cert[256];
  char ssl_ca[256];

  sprintf(ssl_key, "%s/%s", ca_path ,"server-key.pem");
  sprintf(ssl_cert, "%s/%s", ca_path ,"server-cert.pem");
  sprintf(ssl_ca, "%s/%s", ca_path ,"ca.pem");

  printf("ssl_key: %s\n", ssl_key);
  printf("ssl_cert: %s\n", ssl_cert);
  printf("ssl_ca: %s\n", ssl_ca);

  driz_ret = drizzle_set_ssl(con, "invalid_ssl_key", ssl_cert, ssl_ca, ca_path,
    NULL);
  ASSERT_EQ_(DRIZZLE_RETURN_SSL_ERROR, driz_ret, "drizzle_set_ssl(): %s(%s)",
    drizzle_error(con), drizzle_strerror(driz_ret));

  driz_ret = drizzle_set_ssl(con, ssl_key, "invalid_ssl_cert", ssl_ca, ca_path,
    NULL);
  ASSERT_EQ_(DRIZZLE_RETURN_SSL_ERROR, driz_ret, "drizzle_set_ssl(): %s(%s)",
    drizzle_error(con), drizzle_strerror(driz_ret));

  driz_ret = drizzle_set_ssl(con, ssl_key, ssl_cert, "invalid_ssl_ca", ca_path,
    NULL);
  ASSERT_EQ_(DRIZZLE_RETURN_SSL_ERROR, driz_ret, "drizzle_set_ssl(): %s(%s)",
    drizzle_error(con), drizzle_strerror(driz_ret));

  driz_ret = drizzle_set_ssl(con, ssl_key, ssl_cert, ssl_ca, ca_path, "AES128-SHA");
  ASSERT_EQ_(DRIZZLE_RETURN_SSL_ERROR, driz_ret, "drizzle_set_ssl(): %s(%s)",
             drizzle_error(con), drizzle_strerror(driz_ret));

  driz_ret = drizzle_set_ssl(con, ssl_key, ssl_cert, ssl_ca, ca_path, NULL);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "drizzle_set_ssl(): %s(%s)",
             drizzle_error(con), drizzle_strerror(driz_ret));

  ASSERT_EQ_(DRIZZLE_RETURN_OK, drizzle_connect(con), "drizzle_connect() %d, %s(%s)",
    drizzle_error_code(con), drizzle_error(con), drizzle_strerror(driz_ret));
  // SELECT to test that the connection works
  set_up_schema("test_ssl");
  CHECKED_QUERY("CREATE TABLE test_ssl.t1 (a INT)");
  CHECKED_QUERY("INSERT INTO test_ssl.t1 (a) VALUES (1)");
  CHECKED_QUERY("SELECT * FROM test_ssl.t1");
  drizzle_result_buffer(result);
  ASSERT_EQ(drizzle_result_row_count(result), 1);
  ASSERT_EQ(drizzle_result_column_count(result), 1);
  row = drizzle_row_next(result);
  ASSERT_STREQ("1", row[0]);

  drizzle_result_free(result);

  return EXIT_SUCCESS;
}
