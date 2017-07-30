#pragma once

#ifdef __cplusplus
struct drizzle_binlog_query_event_st;
struct drizzle_binlog_xid_event_st;
#endif

#ifdef __cplusplus
extern "C" {
#endif

DRIZZLE_API
drizzle_binlog_xid_event_st *drizzle_binlog_get_xid_event( drizzle_binlog_event_st *event );

DRIZZLE_API
drizzle_binlog_query_event_st *drizzle_binlog_get_query_event( drizzle_binlog_event_st *event );

#ifdef __cplusplus
}
#endif
