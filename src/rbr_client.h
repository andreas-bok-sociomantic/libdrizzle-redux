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
struct drizzle_rbr_client_st
{
    drizzle_binlog_xid_event_st *xid_event;
};
