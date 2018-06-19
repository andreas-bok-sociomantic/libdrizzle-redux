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

#define _GNU_SOURCE

#include "tests/unit/common.h"
#include <yatl/lite.h>

#include <libdrizzle-redux/libdrizzle.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECKED_QUERY(cmd) \
  result = drizzle_query(con, cmd, 0, &driz_ret); \
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, from \"%s\"", \
             drizzle_strerror(driz_ret), drizzle_error(con), cmd);

#define CHECK(s) \
  driz_ret = (s); \
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, in \"%s\"", \
             drizzle_strerror(driz_ret), drizzle_error(con), #s);

/*static const drizzle_column_type_t expected_column_types[10] = {
    DRIZZLE_COLUMN_TYPE_NONE, // Columns are 1-indexed
    DRIZZLE_COLUMN_TYPE_LONG,
    DRIZZLE_COLUMN_TYPE_DATE, DRIZZLE_COLUMN_TYPE_YEAR,
    DRIZZLE_COLUMN_TYPE_TIMESTAMP, DRIZZLE_COLUMN_TYPE_TIMESTAMP,
    DRIZZLE_COLUMN_TYPE_TIME, DRIZZLE_COLUMN_TYPE_TIME,
    DRIZZLE_COLUMN_TYPE_DATETIME,
    //  DRIZZLE_COLUMN_TYPE_NEWDATE,
    DRIZZLE_COLUMN_TYPE_DATETIME,
};*/

/*static const char *column_names[10] = {NULL, "a", "b", "c", "d",
                                       "e",  "f", "g", "h", "i"};*/

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_result_st *result;
  drizzle_return_t driz_ret;
  drizzle_row_t row;
  int num_fields;
  const char *query;
  drizzle_column_st *column;
  drizzle_stmt_st *sth;
  unsigned rows_in_table;

  set_up_connection();
  set_up_schema("test_datatypes");

  SKIP_IF_(drizzle_server_version_number(con) < 50604,
           "Test requires MySQL 5.6.4 or higher");

  CHECKED_QUERY("CREATE TABLE `t1` ("
                "`varchar` VARCHAR( 20 ) NOT NULL ,"
                "`tinyint` TINYINT NOT NULL ,"
                "`text` TEXT NOT NULL ,"
                "`date` DATE NOT NULL ,"
                "`smallint` SMALLINT NOT NULL ,"
                "`mediumint` MEDIUMINT NOT NULL ,"
                "`int` INT NOT NULL ,"
                "`bigint` BIGINT NOT NULL ,"
                "`float` FLOAT( 10, 2 ) NOT NULL ,"
                "`double` DOUBLE NOT NULL ,"
                "`decimal` DECIMAL( 10, 2 ) NOT NULL ,"
                "`datetime` DATETIME NOT NULL ,"
                "`timestamp` TIMESTAMP NOT NULL ,"
                "`time` TIME NOT NULL ,"
                "`year` YEAR NOT NULL ,"
                "`char` CHAR( 10 ) NOT NULL ,"
                "`tinyblob` TINYBLOB NOT NULL ,"
                "`tinytext` TINYTEXT NOT NULL ,"
                "`blob` BLOB NOT NULL ,"
                "`mediumblob` MEDIUMBLOB NOT NULL ,"
                "`mediumtext` MEDIUMTEXT NOT NULL ,"
                "`longblob` LONGBLOB NOT NULL ,"
                "`longtext` LONGTEXT NOT NULL ,"
                /*"`enum` ENUM( '1', '2', '3' ) NOT NULL ,"
                "`set` SET( '1', '2', '3' ) NOT NULL ,"*/
                "`enum` ENUM( '1', '2', '3' ) ,"
                "`set` SET( '1', '2', '3' ) ,"
                "`bool` BOOL NOT NULL ,"
                "`binary` BINARY( 20 ) NOT NULL ,"
                "`varbinary` VARBINARY( 20 ) NOT NULL"
                ") ENGINE = InnoDB DEFAULT CHARSET = utf8;");

  CHECKED_QUERY("INSERT INTO `t1` ("
                "`varchar`, `tinyint`, `text`, `date`,"
                "`smallint`, `mediumint`, `int`, `bigint`,"
                "`float`, `double`, `decimal`, `datetime`,"
                "`timestamp`, `time`, `year`, `char`,"
                "`tinyblob`, `tinytext`, `blob`, `mediumblob`,"
                "`mediumtext`, `longblob`, `longtext`,"
                "`enum`, `set`, `bool`, `binary`, `varbinary`"
                ")"
                "VALUES"
                "("
                "'varchar',"
                "0,"
                "'text',"
                "'2017-04-28',"
                "1,"
                "2,"
                "4,"
                "5,"
                "1.23,"
                "2.3456,"
                "123.4567,"
                "'2017-04-28 13:54:05',"
                "now(),"
                "'13:54:05',"
                "2017,"
                "'char',"
                "'1100',"
                "'tinytext',"
                "'11001100',"
                "'110011001100',"
                "'mediumtext',"
                "'1100110011001100',"
                "'longtext',"
                "('1'),"
                "('4'),"
                "1,"
                "'1100110011001100',"
                "'1100110011001100'"
                ");");
  ASSERT_EQ(drizzle_result_affected_rows(result), 1);

  query = "INSERT INTO t1 ("
          "`varchar`, `tinyint`, `text`, `date`,"
          "`smallint`, `mediumint`, `int`, `bigint`,"
          "`float`, `double`, `decimal`, `datetime`,"
          "`timestamp`, `time`, `year`, `char`,"
          "`tinyblob`, `tinytext`, `blob`, `mediumblob`,"
          "`mediumtext`, `longblob`, `longtext`,"
          "`enum`, `set`, `bool`, `binary`, `varbinary`"
          ")"
          "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

  sth = drizzle_stmt_prepare(con, query, strlen(query), &driz_ret);

  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"",
  drizzle_strerror(driz_ret), drizzle_error(con), query);

  // Set all statement parameters;
  CHECK(drizzle_stmt_set_string(sth, 0, "varchar", strlen("varchar")));
  CHECK(drizzle_stmt_set_tiny(sth, 1, 1, 0));
  CHECK(drizzle_stmt_set_string(sth, 2, "text", strlen("text")));
  CHECK(drizzle_stmt_set_timestamp(sth, 3, 2018, 1, 1, 0, 0, 0, 0));
  CHECK(drizzle_stmt_set_short(sth, 4, 32687, 0));
  CHECK(drizzle_stmt_set_int(sth, 5, 8388351, 0));
  CHECK(drizzle_stmt_set_int(sth, 6, 2147352575, 0));
  CHECK(drizzle_stmt_set_bigint(sth, 7, 9222246136947920895, 0));
  CHECK(drizzle_stmt_set_float(sth, 8, 443664.0f));
  CHECK(drizzle_stmt_set_double(sth, 9, 291.2711110711098l));
  CHECK(drizzle_stmt_set_double(sth, 10, 123.4567));
  CHECK(drizzle_stmt_set_timestamp(sth, 11, 2001, 9, 11, 10, 3, 11, 0));
  CHECK(drizzle_stmt_set_timestamp(sth, 12, 2002, 10, 11, 10, 3, 11, 100));
  CHECK(drizzle_stmt_set_time(sth, 13, 3, 6, 15, 3, 0, 0));
  CHECK(drizzle_stmt_set_int(sth, 14, 2002, 0));
  CHECK(drizzle_stmt_set_string(sth, 15, "char", strlen("char")));
  CHECK(drizzle_stmt_set_string(sth, 16, "1100", strlen("1100")));
  CHECK(drizzle_stmt_set_string(sth, 17, "tinytext", strlen("tinytext")));
  CHECK(drizzle_stmt_set_string(sth, 18, "1100110011001100",
    strlen("1100110011001100")));
  CHECK(drizzle_stmt_set_string(sth, 19, "11001100", strlen("11001100")));
  CHECK(drizzle_stmt_set_string(sth, 20, "mediumtext", strlen("mediumtext")));
  CHECK(drizzle_stmt_set_string(sth, 21, "11001100110011001100110011001100",
    strlen("11001100110011001100110011001100")));
  CHECK(drizzle_stmt_set_string(sth, 22, "longtext", strlen("longtext")));
  /*CHECK(drizzle_stmt_set_string(sth, 23, "('1')", strlen("('1')")));
  CHECK(drizzle_stmt_set_string(sth, 24, "('4')", strlen("('4')")));*/
  CHECK(drizzle_stmt_set_null(sth, 23));
  CHECK(drizzle_stmt_set_null(sth, 24));
  CHECK(drizzle_stmt_set_int(sth, 25, 1, 0));
  CHECK(drizzle_stmt_set_string(sth, 26, "1100110011001100",
    strlen("1100110011001100")));
  CHECK(drizzle_stmt_set_string(sth, 27, "1100110011001100",
    strlen("1100110011001100")));

  driz_ret = drizzle_stmt_execute(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);


  CHECKED_QUERY("SELECT * FROM t1");

  drizzle_result_buffer(result);

  while ((row = drizzle_row_next(result)))
  {
    while ((column= drizzle_column_next(result)))
    {

    }
  }
  drizzle_result_free(result);


  CHECKED_QUERY("DROP TABLE test_datatypes.t1");

  tear_down_schema("test_datatypes");

  return EXIT_SUCCESS;
}
