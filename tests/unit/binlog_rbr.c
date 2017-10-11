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
#include "tests/unit/common.h"

drizzle_result_st *result;

void binlog_error(drizzle_return_t ret, drizzle_st *connection, void *context);
void binlog_error(drizzle_return_t ret, drizzle_st *connection, void *context)
{
    (void) context;
    ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "%s(%s)", drizzle_error(connection),
               drizzle_strerror(ret));
}

void binlog_rbr(drizzle_binlog_rbr_st *rbr, void *context);
void binlog_rbr(drizzle_binlog_rbr_st *rbr, void *context)
{
    (void) context;
    drizzle_binlog_rows_event_st *rows_event;
    size_t rows_count;
    drizzle_binlog_tablemap_event_st * tablemap_event;
    drizzle_return_t ret_val;

    char actual_str[1024];
    uint64_t expected_number;
    const char *schema="test_binlog_rbr";
    const char* table = "binlog_rbr_tbl";

    // xid
    printf("Binlog RBR xid: %ld\n", drizzle_binlog_rbr_xid(rbr));

    // number of row events in binlog group
    tablemap_event = drizzle_binlog_rbr_tablemap_by_tablename(rbr, table);

    if (tablemap_event == NULL )
        return;
    sprintf(actual_str, "%s", drizzle_binlog_tablemap_event_schema_name(tablemap_event));
    if (strcmp(actual_str, schema) != 0)
    {
        printf("skipping\n");
        return;
    }

    sprintf(&actual_str[0], "%s", drizzle_binlog_tablemap_event_table_name(tablemap_event));
    ASSERT_STREQ_(table, actual_str, "Wrong table name. Expected %s got %s",
        table, actual_str);

    unsigned column_count = drizzle_binlog_tablemap_event_column_count(tablemap_event);
    ASSERT_EQ_(column_count, 8, "Wrong number of column in table %s, expected 8 got %d",
               table, column_count);

    rows_count = drizzle_binlog_rbr_row_events_count(rbr, table);
    expected_number = drizzle_binlog_tablemap_event_table_id(tablemap_event);
    printf("table: name=%s, id=%ld, row_count=%ld\n", actual_str, expected_number, rows_count);

    // Get the rows event in the binlog event group
    while ( (rows_event = drizzle_binlog_rbr_rows_event_next(rbr, &ret_val, table) ) != NULL )
    {
        rows_count = drizzle_binlog_rbr_row_events_count(rbr, table);
        ASSERT_EQ_(rows_count, 1, "Wrong number of rows in binlog group. Expected 1 got %ld",
        rows_count);

        // get id of the row event's associated tablemap event
        uint64_t table_id = drizzle_binlog_rows_event_table_id(rows_event);
        ASSERT_EQ(expected_number, table_id);

        // get the row event's associated tablemap event
        tablemap_event =
            drizzle_binlog_rbr_rows_event_tablemap(rbr, rows_event);

        printf("rbr_callback for table %s with id %ld\n",
            drizzle_binlog_tablemap_event_table_name(tablemap_event), table_id);
    }

    for (auto unsigned row_idx = 0; row_idx < rows_count; row_idx++)
    {
        rows_event = drizzle_binlog_rbr_rows_event_index(rbr, row_idx);
        ASSERT_NOT_NULL_(rows_event, "Extracted rows event is NULL");
    }


    drizzle_binlog_rbr_row_events_seek(rbr, DRIZZLE_LIST_BEGIN, table);
    drizzle_binlog_rbr_row_events_seek(rbr, DRIZZLE_LIST_END, table);
    printf("FINISHED\n");
}


int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;
    drizzle_binlog_st *binlog;
    drizzle_return_t ret;
    drizzle_return_t driz_ret;

    set_up_connection();
    set_up_schema("test_binlog_rbr");

    CHECKED_QUERY("CREATE TABLE test_binlog_rbr.binlog_rbr_tbl"
                  "(a INT PRIMARY KEY AUTO_INCREMENT, "
                  "b TINYINT, c SMALLINT, d MEDIUMINT, e INT, f BIGINT, g FLOAT, "
                  "h DOUBLE(16,13))");

    CHECKED_QUERY("INSERT INTO test_binlog_rbr.binlog_rbr_tbl "
                  "(b,c,d,e,f,g,h) VALUES "
                  "(1,1,1,1,1,1,1)");

    char *binlog_file;
    uint32_t end_position;
    ret = drizzle_binlog_get_filename(con, &binlog_file, &end_position, -1);
    ASSERT_EQ_(DRIZZLE_RETURN_OK, ret,
               "Couldn't retrieve binlog filename: %s(%s)",
               drizzle_error(con), drizzle_strerror(ret));

    binlog = drizzle_binlog_rbr_init(con, binlog_rbr, binlog_error, NULL, true);
    ASSERT_NOT_NULL_(binlog, "Binlog object creation error");

    ret = drizzle_binlog_start(binlog, 0, binlog_file, 0);

    SKIP_IF_(ret == DRIZZLE_RETURN_ERROR_CODE, "Binlog is not open?: %s(%s)",
             drizzle_error(con), drizzle_strerror(ret));
    ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "Drizzle binlog start failure: %s(%s)",
               drizzle_error(con), drizzle_strerror(ret));

    free(binlog_file);

    CHECKED_QUERY("DROP TABLE test_binlog_rbr.binlog_rbr_tbl");

    tear_down_schema("test_binlog_rbr");
    return EXIT_SUCCESS;
} /* main */
