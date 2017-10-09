#include "config.h"
#include "src/common.h"
#include <cstdarg>


drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_st::get_tablemap_event(
    uint64_t table_id)
{
    auto table_id_ = table_id == 0 ? this->current_tablemap_id : table_id;

    if (tablemap_events.find(table_id_) == tablemap_events.end())
    {
        return NULL;
    }

    return tablemap_events.find(table_id_)->second;
}

drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_st::get_tablemap_event(const char* table_name, ...)
{
    va_list args;
    const char *schema_name = NULL;
    va_start(args, table_name);
    table_name = va_arg(args, const char*);
    va_end(args);

    schema_name = schema_name == NULL ? db : schema_name;

    sprintf(&fmt_buffer[0], "%s.%s", schema_name, table_name);

    if (tablename_tablemap_event.find(fmt_buffer) == tablename_tablemap_event.end())
    {
        return NULL;
    }
    return tablename_tablemap_event.find(fmt_buffer)->second;
}

drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_st::create_tablemap_event(
    uint64_t table_id)
{
    if (tablemap_events.find(table_id) == tablemap_events.end())
    {
        tablemap_events.insert(std::make_pair(table_id,
                                              new
                                              drizzle_binlog_tablemap_event_st()));
    }

    this->current_tablemap_id = table_id;
    return tablemap_events.find(table_id)->second;
}

drizzle_binlog_rows_event_st *drizzle_binlog_rbr_st::get_rows_event()
{
    if ( row_events_count_ == 0 )
    {
        return NULL;
    }

    return rows_events.at(row_events_count_ - 1);
}

drizzle_binlog_rows_event_st *drizzle_binlog_rbr_st::create_rows_event()
{
    if (row_events_count_ + 1 > rows_events.size())
    {
        rows_events.push_back(new drizzle_binlog_rows_event_st());
    }

    // increment the number of parsed rows
    rows_events_parsed++;

    auto *rows_event = rows_events.at(row_events_count_);

    // Reuse the rows event if the client has not set the rbr callback function
    row_events_count_ = binlog_rbr_fn != NULL ? row_events_count_ + 1 : 1;

    return rows_event;
}

void drizzle_binlog_rbr_st::add_table_row_mapping(
    drizzle_binlog_rows_event_st *rows_event)
{
    if (binlog_rbr_fn != NULL )
    {
        tablename_rows_events.add_mapping(rows_event);
    }
}


void drizzle_binlog_rbr_st::add_binlog_event(drizzle_binlog_event_st *event)
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
        reset();
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
} // drizzle_binlog_rbr_st::add_binlog_event

void drizzle_binlog_rbr_st::reset(bool free_all)
{
    if (free_all)
    {
        rows_events.clear();
        for (auto kv : tablemap_events)
        {
            free(kv.second);
        }
        tablemap_events.clear();
    }

    // clear the mapping between tables and rows events
    tablename_rows_events.reset();

    rows_event_it.it = rows_events.end();
    row_events_count_ = 0;
    current_tablemap_id = 0;
    rows_events_parsed = 0;
    rows_event_it.reset();
}

size_t drizzle_binlog_rbr_st::get_row_events_count(const char *table_name)
{
    return tablename_rows_events.row_events_count(table_name);
}

size_t drizzle_binlog_rbr_row_events_count(drizzle_binlog_rbr_st *binlog_rbr,
                                           ...)
{

    const char *table_name = NULL;
    va_list args;

    va_start(args, binlog_rbr);
    table_name = va_arg(args, const char *);
    va_end(args);

    if (table_name == NULL)
    {
        return binlog_rbr->row_events_count_;
    }
    else
    {
        return binlog_rbr->get_row_events_count(table_name);
    }
}


uint64_t drizzle_binlog_rbr_xid(drizzle_binlog_rbr_st *binlog_rbr)
{
    return binlog_rbr->xid_event.xid;
}


drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_next(
    drizzle_binlog_rbr_st *binlog_rbr, drizzle_return_t *ret_ptr, ...)
{
    drizzle_binlog_rows_event_st *rows_event;
    const char *table_name = NULL;
    va_list args;

    va_start(args, ret_ptr);
    table_name = va_arg(args, const char *);
    va_end(args);

    if (table_name[0] != '\0')
    {
        printf("Table name %s\n", table_name);
        rows_event =
            binlog_rbr->tablename_rows_events.next_row_event(table_name);
        *ret_ptr = rows_event ==
            NULL ? DRIZZLE_RETURN_ROW_END : DRIZZLE_RETURN_OK;
        return rows_event;
    }

    if (!binlog_rbr->rows_event_it.active)
    {
        binlog_rbr->rows_event_it.it = binlog_rbr->rows_events.begin();
        binlog_rbr->rows_event_it.active = true;
    }

    if (binlog_rbr->rows_event_it.it == binlog_rbr->rows_events.end())
    {
        *ret_ptr = DRIZZLE_RETURN_ROW_END;
        return NULL;
    }

    rows_event = *binlog_rbr->rows_event_it.it;
    binlog_rbr->rows_event_it.it++;
    *ret_ptr = DRIZZLE_RETURN_OK;
    return rows_event;
} // drizzle_binlog_rbr_rows_event_next

drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_prev(
    drizzle_binlog_rbr_st *binlog_rbr, drizzle_return_t *ret_ptr, ...)
{
    if (prev(binlog_rbr->rows_event_it.it) <=
        binlog_rbr->rows_events.begin() - 1)
    {
        binlog_rbr->rows_event_it.it = binlog_rbr->rows_events.begin() - 1;
        *ret_ptr = DRIZZLE_RETURN_ROW_REND;
        return NULL;
    }

    binlog_rbr->rows_event_it.it--;
    drizzle_binlog_rows_event_st *rows_event = *binlog_rbr->rows_event_it.it;
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
    return binlog_rbr->rows_event_it.it >= binlog_rbr->rows_events.end() ||
           binlog_rbr->rows_event_it.it < binlog_rbr->rows_events.begin() ? -1 :
           distance(
        binlog_rbr->rows_events.begin(), binlog_rbr->rows_event_it.it);
}

drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_tablemap_event(
    drizzle_binlog_rbr_st *binlog_rbr, uint64_t table_id)
{
    if ( table_id == 0 )
    {
        drizzle_set_error(binlog_rbr->binlog->con, __FILE_LINE_FUNC__,
                          "table id must be greater than 0");
        return NULL;
    }
    return binlog_rbr->get_tablemap_event(table_id);
}



drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_rows_event_tablemap(
    drizzle_binlog_rbr_st *binlog_rbr, drizzle_binlog_rows_event_st *event)
{
    return binlog_rbr->get_tablemap_event(event->table_id);
}


drizzle_return_t drizzle_binlog_rbr_row_events_seek(drizzle_binlog_rbr_st *binlog_rbr,
    drizzle_list_position_t pos, ...)
{
    const char *table_name = NULL;
    va_list args;

    va_start(args, pos);
    table_name = va_arg(args, const char *);
    va_end(args);

    if (table_name[0] != '\0')
    {
        if (binlog_rbr->tablename_rows_events.has_table(table_name))
        {
            binlog_rbr->tablename_rows_events.set_rows_events_it(table_name,
                pos);
        }
        else
        {
            sprintf(binlog_rbr->binlog->con->last_error,
                "table: %s does not exist ", table_name);
            return DRIZZLE_RETURN_INVALID_ARGUMENT;
        }
    }
    else
    {
        binlog_rbr->rows_event_it.it = pos == DRIZZLE_LIST_BEGIN ?
            binlog_rbr->rows_events.begin() : binlog_rbr->rows_events.end();
        binlog_rbr->rows_event_it.active = false;
    }

    return DRIZZLE_RETURN_OK;
}
