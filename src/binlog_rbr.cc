#include "config.h"
#include "src/common.h"

uint64_t drizzle_binlog_rbr_xid(drizzle_binlog_rbr_st *binlog_rbr)
{
    return binlog_rbr->xid_event.xid;
}
