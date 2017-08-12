/*
 * Copyright (C) 2017 Andreas Bok Andersen (andreas.bok@sociomantic.com)
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */


/**
 *
 * Class which models RowBasedReplication
 *
 * Collects binlog events
 *
 * Callback to client when a valid event group has been received
 *
 * Return a iterator
 *
 * @brief [brief description]
 * @details [long description]
 * @return [description]
 */

#include <yatl/lite.h>

#include "tests/unit/common.h"

void binlog_error(drizzle_return_t ret, drizzle_st *connection, void *context)
{
    (void) context;
    ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "%s(%s)", drizzle_error(connection),
               drizzle_strerror(ret));
}


void binlog_event(drizzle_binlog_event_st *event, void *context)
{
    (void) context;
    (void) event;
    // drizzle_binlog_event_types_t type = drizzle_binlog_event_type(event);
    // printf("%s\n", drizzle_binlog_event_type_str(type));
}

void rbr_callback(drizzle_binlog_rbr_st *rbr, void *context)
{
    (void) context;
    (void) rbr;
    drizzle_binlog_xid_event_st *xid_event = drizzle_binlog_rbr_get_xid_event(
        rbr);
    printf("rbr_callback: xid %" PRIu64 "\n", xid_event->xid());
}

int main(int argc, char const *argv[])
{
    (void) argc;
    (void) argv;
    drizzle_binlog_st *binlog;
    drizzle_return_t driz_ret;
    drizzle_result_st VARIABLE_IS_NOT_USED *result;
    set_up_connection();

    char *binlog_file;
    uint32_t end_position;
    driz_ret =
        drizzle_binlog_get_filename(con, &binlog_file, &end_position, -1);
    ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret,
               "Couldn't retrieve binlog filename: %s(%s)",
               drizzle_error(con), drizzle_strerror(driz_ret));

    printf("pos %d\n", end_position);
    binlog = drizzle_binlog_init(con, binlog_event, binlog_error, NULL, true);
    drizzle_binlog_set_rbr_fn(binlog, rbr_callback, NULL);
    driz_ret = drizzle_binlog_start(binlog, 0, binlog_file, 0);

    SKIP_IF_(driz_ret == DRIZZLE_RETURN_ERROR_CODE,
             "Binlog is not open?: %s(%s)",
             drizzle_error(con), drizzle_strerror(driz_ret));
    ASSERT_EQ_(DRIZZLE_RETURN_EOF, driz_ret,
               "Drizzle binlog start failure: %s(%s)",
               drizzle_error(con), drizzle_strerror(driz_ret));

    free(binlog_file);
    return EXIT_SUCCESS;
}

