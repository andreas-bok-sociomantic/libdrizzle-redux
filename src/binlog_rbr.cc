#include "config.h"
#include "src/common.h"

drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_st::get_tablemap_event(uint64_t table_id)
{
    auto table_id_ = table_id == 0 ? this->current_tablemap_id : table_id;
    if (tablemap_events.find(table_id_) == tablemap_events.end())
    {
        return NULL;
    }

    return tablemap_events.find(table_id_)->second;
}

drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_st::add_tablemap_event(
    uint64_t table_id)
{
    if (tablemap_events.find(table_id) == tablemap_events.end())
    {
        tablemap_events.insert(std::make_pair(table_id, new drizzle_binlog_tablemap_event_st()));
    }

    this->current_tablemap_id = table_id;
    return tablemap_events.find(table_id)->second;
}

drizzle_binlog_rows_event_st * drizzle_binlog_rbr_st::get_rows_event()
{
    if ( row_events_count_ == 0 )
    {
        return NULL;
    }

    return rows_events.at(row_events_count_ - 1);
}

drizzle_binlog_rows_event_st *drizzle_binlog_rbr_st::add_rows_event()
{
    if (row_events_count_ + 1 > rows_events.size())
    {
        rows_events.push_back(new drizzle_binlog_rows_event_st());
    }

    // increment the number of parsed rows
    rows_events_parsed++;

    auto *rows_event = rows_events.at(row_events_count_);
    // Reuse the rows event if the client has not set the rbr callback function
    if ( binlog_rbr_fn != NULL )
    {
        row_events_count_++;
    }

    return rows_event;
}

void drizzle_binlog_rbr_st::add_table_row_mapping(drizzle_binlog_rows_event_st *rows_event)
{
    if (binlog_rbr_fn != NULL )
    {
        if ( map_tablename_rows_events.find(rows_event->table_name) ==
            map_tablename_rows_events.end() )
        {
            vec_ptr_row_events vec;
            map_tablename_rows_events.insert(
                std::make_pair(rows_event->table_name, vec));
        }

        auto vec_rows = &map_tablename_rows_events.find(rows_event->table_name)->second;
        vec_rows->push_back(&rows_event);
    }
}


void drizzle_binlog_rbr_st::add_binlog_event(drizzle_binlog_event_st* event)
{
    if (event->type == DRIZZLE_EVENT_TYPE_XID)
    {
        drizzle_binlog_parse_xid_event(event);
        if (binlog_rbr_fn != NULL)
        {
            binlog_rbr_fn(this, binlog->binlog_context);
        }
    }
    else if (event->type == DRIZZLE_EVENT_TYPE_QUERY)
    {
        drizzle_binlog_parse_query_event(event);
    }
    else if (drizzle_binlog_is_rows_event(event->type))
    {
        drizzle_binlog_parse_rows_event(event);
    }
    else if (event->type == DRIZZLE_EVENT_TYPE_TABLE_MAP)
    {
        drizzle_binlog_parse_tablemap_event(event);
    }
    else
    {
        return;
    }
}

void drizzle_binlog_rbr_st::reset(bool free_rows)
{
    if (free_rows)
    {
        rows_events.clear();
    }

    rows_event_it = rows_events.end();
    row_events_count_ = 0;
    tablemap_events.clear();
}

size_t drizzle_binlog_rbr_row_events_count(drizzle_binlog_rbr_st *binlog_rbr)
{
    return binlog_rbr->row_events_count_;
}


uint64_t drizzle_binlog_rbr_xid(drizzle_binlog_rbr_st *binlog_rbr)
{
    return binlog_rbr->xid_event.xid;
}


drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_next(
    drizzle_binlog_rbr_st *binlog_rbr, drizzle_return_t *ret_ptr)
{
    if (next(binlog_rbr->rows_event_it) >= binlog_rbr->rows_events.end())
    {
        next(binlog_rbr->rows_event_it) = binlog_rbr->rows_events.end();
        *ret_ptr = DRIZZLE_RETURN_ROW_END;
        return NULL;
    }

    binlog_rbr->rows_event_it++;
    drizzle_binlog_rows_event_st *rows_event = *binlog_rbr->rows_event_it;
    *ret_ptr = DRIZZLE_RETURN_OK;
    return rows_event;
}

drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_prev(
    drizzle_binlog_rbr_st *binlog_rbr, drizzle_return_t *ret_ptr)
{
    if (prev(binlog_rbr->rows_event_it) <= binlog_rbr->rows_events.begin() - 1)
    {
        binlog_rbr->rows_event_it = binlog_rbr->rows_events.begin() - 1;
        *ret_ptr = DRIZZLE_RETURN_ROW_REND;
        return NULL;
    }

    binlog_rbr->rows_event_it--;
    drizzle_binlog_rows_event_st *rows_event = *binlog_rbr->rows_event_it;
    *ret_ptr = DRIZZLE_RETURN_OK;
    return rows_event;
}

drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_index(
    drizzle_binlog_rbr_st *binlog_rbr, uint64_t row_event_idx)
{
    if ( row_event_idx >= binlog_rbr->row_events_count_ )
    {
        return NULL;
    }

    return binlog_rbr->rows_events.at(row_event_idx);
}

int64_t drizzle_binlog_rbr_rows_event_current(drizzle_binlog_rbr_st *binlog_rbr)
{
    return binlog_rbr->rows_event_it >= binlog_rbr->rows_events.end() ||
     binlog_rbr->rows_event_it < binlog_rbr->rows_events.begin() ? -1 :
        distance(binlog_rbr->rows_events.begin(), binlog_rbr->rows_event_it);
}
