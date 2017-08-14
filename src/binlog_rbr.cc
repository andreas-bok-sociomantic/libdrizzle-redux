/**
 * @brief      Binlog Row based replication structure
 *
 * @description
 */

#include "config.h"
#include "src/common.h"

drizzle_binlog_rbr_st::drizzle_binlog_rbr_st()
{
    this->xid_event = new (std::nothrow) drizzle_binlog_xid_event_st();
    this->query_event = new (std::nothrow) drizzle_binlog_query_event_st();
    this->rbr_fn = NULL;
}

drizzle_binlog_rbr_st::~drizzle_binlog_rbr_st()
{
    delete this->xid_event;
    delete this->query_event;
    for (iterator_table_map_events it= this->table_map_events.begin();
        it != this->table_map_events.end(); it++)
    {
        delete it->second;
    }
    this->table_map_events.clear();

    for (auto it = this->rows_events.begin(); it != rows_events.end(); it++)
    {
        delete *it;
    }
    this->rows_events.clear();
}

drizzle_binlog_xid_event_st *drizzle_binlog_rbr_get_xid_event(
    drizzle_binlog_rbr_st* rbr)
{
    return rbr->xid_event;
}

drizzle_binlog_query_event_st *drizzle_binlog_rbr_get_query_event(
    drizzle_binlog_rbr_st* rbr)

{
    return rbr->query_event;
}

drizzle_return_t drizzle_binlog_set_rbr_fn(drizzle_binlog_st *binlog,
    drizzle_binlog_rbr_fn *rbr_fn, void *context)
{
    if (binlog == NULL)
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    if (binlog->rbr == NULL)
    {
        binlog->rbr = new (std::nothrow) drizzle_binlog_rbr_st();
    }

    binlog->rbr->rbr_fn = rbr_fn;
    binlog->rbr->context = context;

    return DRIZZLE_RETURN_OK;
}

void drizzle_binlog_rbr_st::add_event(drizzle_binlog_event_st *event)
{
    if (event->type == DRIZZLE_EVENT_TYPE_QUERY)
    {
        this->query_event = drizzle_binlog_get_query_event(event);
    }
    else if (event->type == DRIZZLE_EVENT_TYPE_XID)
    {
        this->xid_event = drizzle_binlog_get_xid_event(event);
        if (this->rbr_fn != NULL)
        {
            this->rbr_fn(this, this->context);
        }
    }
}
