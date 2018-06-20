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
#include <float.h>
#include <math.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

#define CHECKED_QUERY(cmd) \
  result = drizzle_query(con, cmd, 0, &driz_ret); \
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, from \"%s\"", \
             drizzle_strerror(driz_ret), drizzle_error(con), cmd);

#define CHECK(s) \
  driz_ret = (s); \
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, in \"%s\"", \
             drizzle_strerror(driz_ret), drizzle_error(con), #s);

struct test_column_st
{
  const char name[1024];
  const char value[128];
  const drizzle_column_type_t type;
};

typedef struct test_column_st test_column_st;
test_column_st test_columns[] = {
  {"varchar", "varchar", DRIZZLE_COLUMN_TYPE_VAR_STRING},
  {"tinyint", "0", DRIZZLE_COLUMN_TYPE_TINY},
  {"text", "text", DRIZZLE_COLUMN_TYPE_BLOB},
  {"date", "2017-04-28", DRIZZLE_COLUMN_TYPE_DATE},
  {"smallint", "32687", DRIZZLE_COLUMN_TYPE_SHORT},
  {"mediumint", "8388351", DRIZZLE_COLUMN_TYPE_INT24},
  {"int", "2147352575", DRIZZLE_COLUMN_TYPE_LONG},
  {"bigint", "9222246136947920895", DRIZZLE_COLUMN_TYPE_LONGLONG},
  {"float", "1.23", DRIZZLE_COLUMN_TYPE_FLOAT},
  {"double", "2.3456", DRIZZLE_COLUMN_TYPE_DOUBLE},
  {"decimal", "123.46", DRIZZLE_COLUMN_TYPE_NEWDECIMAL},
  {"datetime", "2017-04-28 13:54:05", DRIZZLE_COLUMN_TYPE_DATETIME},
  {"timestamp", "2018-03-12 00:00:00", DRIZZLE_COLUMN_TYPE_TIMESTAMP},
  {"time", "13:54:05", DRIZZLE_COLUMN_TYPE_TIME},
  {"year", "2017", DRIZZLE_COLUMN_TYPE_YEAR},
  {"char", "char", DRIZZLE_COLUMN_TYPE_STRING},
  {"tinyblob", "1100", DRIZZLE_COLUMN_TYPE_BLOB},
  {"tinytext", "tinytext", DRIZZLE_COLUMN_TYPE_BLOB},
  {"blob", "11001100", DRIZZLE_COLUMN_TYPE_BLOB},
  {"mediumblob", "110011001100", DRIZZLE_COLUMN_TYPE_BLOB},
  {"mediumtext", "mediumtext", DRIZZLE_COLUMN_TYPE_BLOB},
  {"longblob", "1100110011001100", DRIZZLE_COLUMN_TYPE_BLOB},
  {"longtext", "longtext", DRIZZLE_COLUMN_TYPE_BLOB},
  {"enum", "1", DRIZZLE_COLUMN_TYPE_STRING},
  {"set", "2,3", DRIZZLE_COLUMN_TYPE_STRING},
  {"bool", "1", DRIZZLE_COLUMN_TYPE_TINY},
  {"binary", "1100110011001100", DRIZZLE_COLUMN_TYPE_STRING},
  {"varbinary", "1100110011001100", DRIZZLE_COLUMN_TYPE_VAR_STRING}
};

void CHECK_PARAM(drizzle_stmt_st *stmt, int col_idx);
void CHECK_PARAM(drizzle_stmt_st *stmt, int col_idx)
{
  size_t len;
  drizzle_return_t driz_ret;
  const char *col_strval = NULL;
  uint32_t actual_intval;
  uint32_t expected_intval;
  uint64_t actual_bigintval;
  uint64_t expected_bigintval;
  double actual_floatval;
  double expected_floatval;
  test_column_st expected = test_columns[col_idx];
      switch(expected.type)
      {
        case DRIZZLE_COLUMN_TYPE_NULL:
          /* I don't think this one should happen, but cover it anyway */
          break;
        case DRIZZLE_COLUMN_TYPE_TINY:
        case DRIZZLE_COLUMN_TYPE_SHORT:
        case DRIZZLE_COLUMN_TYPE_YEAR:
        case DRIZZLE_COLUMN_TYPE_INT24:
        case DRIZZLE_COLUMN_TYPE_LONG:
          expected_intval = atoi(expected.value);
          actual_intval = drizzle_stmt_get_int(stmt, col_idx, &driz_ret);
          ASSERT_EQ(expected_intval, actual_intval);
          actual_intval = drizzle_stmt_get_int_from_name(stmt, expected.name,
            &driz_ret);
          ASSERT_EQ(expected_intval, actual_intval);
          break;
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
          actual_bigintval = drizzle_stmt_get_bigint(stmt, col_idx, &driz_ret);
          expected_bigintval = strtoull(expected.value, NULL, 10);
          ASSERT_EQ(actual_bigintval, expected_bigintval);
          actual_bigintval = drizzle_stmt_get_bigint_from_name(stmt, expected.name, &driz_ret);
          ASSERT_EQ(actual_bigintval, expected_bigintval);
          break;
        case DRIZZLE_COLUMN_TYPE_FLOAT:
        case DRIZZLE_COLUMN_TYPE_DOUBLE: {
          actual_floatval = drizzle_stmt_get_double(stmt, col_idx, &driz_ret);
          expected_floatval = strtod(expected.value, NULL);
          double epsilon = fabs(actual_floatval-expected_floatval);
          bool isequal = expected.type == DRIZZLE_COLUMN_TYPE_FLOAT ?
              epsilon < FLT_EPSILON :epsilon < DBL_EPSILON;
          ASSERT_TRUE(isequal);
          actual_floatval = drizzle_stmt_get_double_from_name(stmt, expected.name, &driz_ret);
          epsilon = fabs(actual_floatval-expected_floatval);
          isequal = expected.type == DRIZZLE_COLUMN_TYPE_FLOAT ?
              epsilon < FLT_EPSILON :epsilon < DBL_EPSILON;
          ASSERT_TRUE(isequal);

          actual_intval = drizzle_stmt_get_int(stmt, col_idx, &driz_ret);
          ASSERT_EQ(driz_ret, DRIZZLE_RETURN_TRUNCATED);
          }
          break;
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
          printf("Skipping unsupported column of type: DRIZZLE_COLUMN_TYPE_NEWDECIMAL\n");
          break;
        case DRIZZLE_COLUMN_TYPE_BLOB:
        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_DATETIME:
        case DRIZZLE_COLUMN_TYPE_STRING:
        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
          col_strval = drizzle_stmt_get_string(stmt, col_idx, &len, &driz_ret);
          ASSERT_STREQ_(col_strval, expected.value,
          "Retrieved bad column[%d] %s value %s - expected %s", col_idx, drizzle_column_type_str(expected.type), col_strval,
          expected.value);
          col_strval = drizzle_stmt_get_string_from_name(stmt, expected.name, &len, &driz_ret);
          ASSERT_STREQ_(col_strval, expected.value,
          "Retrieved bad column[%d] %s value %s - expected %s", col_idx, drizzle_column_type_str(expected.type), col_strval,
          expected.value);
          break;
        default:
          printf("Retrieved bad column[%d] %s value %s - expected %s\n", col_idx, drizzle_column_type_str(expected.type), col_strval,
          expected.value);
          break;
      }
}
#define CHECK_INSERT_STMT(actual, expected, column_st, stmt, col_idx) \
  check_column_value(actual, expected, column_st, stmt, col_idx);

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_result_st *result;
  drizzle_return_t driz_ret;
  drizzle_row_t row;
  const char *query;
  drizzle_column_st *column;
  drizzle_stmt_st *sth;

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
                "`tinyblob` TINYBLOB NOT NULL,"
                "`tinytext` TINYTEXT NOT NULL ,"
                "`blob` BLOB NOT NULL ,"
                "`mediumblob` MEDIUMBLOB NOT NULL,"
                "`mediumtext` MEDIUMTEXT NOT NULL ,"
                "`longblob` LONGBLOB NOT NULL ,"
                "`longtext` LONGTEXT NOT NULL ,"
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
                "32687,"
                "8388351,"
                "2147352575,"
                "9222246136947920895,"
                "1.23,"
                "2.3456,"
                "123.4567,"
                "'2017-04-28 13:54:05',"
                "TIMESTAMP('2018-03-12'),"
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
                "'1',"
                "'2,3',"
                "1,"
                "'1100110011001100',"
                "'1100110011001100'"
                ");");
  ASSERT_EQ(drizzle_result_affected_rows(result), 1);

  CHECKED_QUERY("SELECT * FROM t1");

  drizzle_result_buffer(result);

  ASSERT_EQ_(drizzle_result_column_count(result), 28,
    "Invalid number of columns in resultset");

  while ((row = drizzle_row_next(result)))
  {
    int col_idx = 0;
    test_column_st *expected;
    while ((column= drizzle_column_next(result)))
    {
      char *actual = row[col_idx];
      expected = &test_columns[col_idx];

      ASSERT_STREQ_(actual, expected->value,
        "Retrieved bad row value %s - expected %s", actual,
        expected->value);
      ASSERT_EQ_(drizzle_column_type(column), expected->type,
      "Column type 'DRIZZLE_COLUMN_TYPE_%s' resolved to wrong name: '%s'",
      drizzle_column_type_str(drizzle_column_type(column)),
      drizzle_column_type_str(expected->type));

      /*CHECK_INSERT_QUERY(row[col_idx], test_columns[col_idx], column);*/
      col_idx++;
    }
  }
  drizzle_result_free(result);

  //CHECKED_QUERY("DELETE FROM test_datatypes.t1");

  query = "INSERT INTO t1 ("
          "`varchar`,"
          "`tinyint`,"
          "`text`,"
          "`date`,"
          "`smallint`,"
          "`mediumint`,"
          "`int`,"
          "`bigint`,"
          "`float`,"
          "`double`,"
          "`decimal`,"
          "`datetime`,"
          "`timestamp`,"
          "`time`,"
          "`year`,"
          "`char`,"
          "`tinyblob`,"
          "`tinytext`,"
          "`blob`,"
          "`mediumblob`,"
          "`mediumtext`,"
          "`longblob`,"
          "`longtext`,"
          "`enum`,"
          "`set`,"
          "`bool`,"
          "`binary`,"
          "`varbinary`"
          ") "
          "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

  sth = drizzle_stmt_prepare(con, query, strlen(query), &driz_ret);

  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"",
  drizzle_strerror(driz_ret), drizzle_error(con), query);

  // Set all statement parameters;
  CHECK(drizzle_stmt_set_string(sth, 0, "varchar", strlen("varchar")));
  CHECK(drizzle_stmt_set_tiny(sth, 1, 0, 0));
  CHECK(drizzle_stmt_set_string(sth, 2, "text", strlen("text")));
  CHECK(drizzle_stmt_set_timestamp(sth, 3, 2017, 4, 28, 0, 0, 0, 0));
  CHECK(drizzle_stmt_set_short(sth, 4, 32687, 0));
  CHECK(drizzle_stmt_set_int(sth, 5, 8388351, 0));
  CHECK(drizzle_stmt_set_int(sth, 6, 2147352575, 0));
  CHECK(drizzle_stmt_set_bigint(sth, 7, 9222246136947920895, 0));
  CHECK(drizzle_stmt_set_float(sth, 8, 1.23f));
  CHECK(drizzle_stmt_set_double(sth, 9, 2.3456l));
  CHECK(drizzle_stmt_set_double(sth, 10, 123.4567l));
  CHECK(drizzle_stmt_set_timestamp(sth, 11, 2017, 4, 28, 13, 54, 05, 0));
  CHECK(drizzle_stmt_set_timestamp(sth, 12, 2018, 03, 12, 0, 0, 0, 0));
  CHECK(drizzle_stmt_set_time(sth, 13, 0, 13, 54, 5, 0, 0));
  CHECK(drizzle_stmt_set_int(sth, 14, 2017, 0));
  CHECK(drizzle_stmt_set_string(sth, 15, "char", strlen("char")));
  CHECK(drizzle_stmt_set_string(sth, 16, "1100", 4));
  CHECK(drizzle_stmt_set_string(sth, 17, "tinytext", strlen("tinytext")));
  CHECK(drizzle_stmt_set_string(sth, 18, "11001100", strlen("11001100")));
  CHECK(drizzle_stmt_set_string(sth, 19, "110011001100", strlen("110011001100")));
  CHECK(drizzle_stmt_set_string(sth, 20, "mediumtext", strlen("mediumtext")));
  CHECK(drizzle_stmt_set_string(sth, 21, "1100110011001100", strlen("1100110011001100")));
  /*CHECK(drizzle_stmt_set_string(sth, 21, "0x11001100110011001100110011001100",
    strlen("11001100110011001100110011001100")));*/
  CHECK(drizzle_stmt_set_string(sth, 22, "longtext", strlen("longtext")));
  CHECK(drizzle_stmt_set_string(sth, 23, "1", 1));
  CHECK(drizzle_stmt_set_string(sth, 24, "2,3", 3));
  CHECK(drizzle_stmt_set_int(sth, 25, 1, 0));
  /*char value_buffer[128];
  drizzle_hex_string(value_buffer, (const unsigned char *) "binary", strlen("binary"));
  CHECK(drizzle_stmt_set_string(sth, 26,  value_buffer, strlen(value_buffer)));*/
  CHECK(drizzle_stmt_set_string(sth, 26,  "1100110011001100", strlen("1100110011001100")));
  CHECK(drizzle_stmt_set_string(sth, 27, "1100110011001100", strlen("1100110011001100")));

  driz_ret = drizzle_stmt_execute(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);
  driz_ret = drizzle_stmt_buffer(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, buffering \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);

  ASSERT_EQ(drizzle_stmt_affected_rows(sth), 1);

  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "%s", drizzle_error(con));

  CHECK(drizzle_stmt_close(sth));

  query = "SELECT "
          "`varchar`,"
          "`tinyint`,"
          "`text`,"
          "`date`,"
          "`smallint`,"
          "`mediumint`,"
          "`int`,"
          "`bigint`,"
          "`float`,"
          "`double`,"
          "`decimal`,"
          "`datetime`,"
          "`timestamp`,"
          "`time`,"
          "`year`,"
          "`char`,"
          "`tinyblob`,"
          "`tinytext`,"
          "`blob`,"
          "`mediumblob`,"
          "`mediumtext`,"
          "`longblob`,"
          "`longtext`,"
          "`enum`,"
          "`set`,"
          "`bool`,"
          "`binary`,"
          "`varbinary`"
          "FROM test_datatypes.t1";

  sth = drizzle_stmt_prepare(con, query, strlen(query), &driz_ret);

  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"",
  drizzle_strerror(driz_ret), drizzle_error(con), query);

  driz_ret = drizzle_stmt_execute(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);
  driz_ret = drizzle_stmt_buffer(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, buffering \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);
  int column_count= drizzle_stmt_column_count(sth);

  ASSERT_EQ(column_count, 28);

  while (drizzle_stmt_fetch(sth) != DRIZZLE_RETURN_ROW_END) {
    int col_idx = 0;

    while (col_idx < column_count)
    {
      CHECK_PARAM(sth, col_idx);
      col_idx++;
    }
  }

  CHECK(drizzle_stmt_close(sth));

  CHECKED_QUERY("DROP TABLE test_datatypes.t1");

  tear_down_schema("test_datatypes");

  return EXIT_SUCCESS;
}
#pragma GCC diagnostic pop
