#include "config.h"
#include "src/common.h"

drizzle_binlog_event_st* drizzle_binlog_xid_event_header(
    drizzle_binlog_xid_event_st* event)
{
    return &event->header;
}

uint64_t drizzle_binlog_xid_event_xid(drizzle_binlog_xid_event_st* event)
{
    return event->xid;
}


drizzle_binlog_xid_event_st *drizzle_binlog_get_xid_event(
    drizzle_binlog_event_st *event)
{
    auto *xid_event = &event->binlog_rbr->xid_event;
    set_event_header(&xid_event->header, event);
    xid_event->xid = (uint64_t) drizzle_get_byte8(event->data_ptr);
    event->data_ptr += 8;
    return xid_event;
}
