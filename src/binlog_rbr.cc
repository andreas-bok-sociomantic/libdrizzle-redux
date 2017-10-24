#include "config.h"
#include "src/common.h"
// #include <cstdarg>
#include <stdarg.h>

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

drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_st::get_tablemap_event(
    const char *table_name)
{
    auto table_id = tableid_by_tablename(table_name);

    return table_id != 0 ? get_tablemap_event(table_id) : NULL;
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

void drizzle_binlog_rbr_st::add_tablemap_event(
    drizzle_binlog_tablemap_event_st *event)
{
    std::string schema_table(schema_table_name(event->table_name));
    if (tableid_by_tablename(schema_table.c_str()) == 0)
    {
        tablename_tableid.insert(std::make_pair(schema_table,
                                                event->table_id));
    }
    else
    {
        tablename_tableid[schema_table] = event->table_id;
    }
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

    auto rows_event = rows_events.at(row_events_count_);
    rows_event->reset();

    // Reuse the rows event if the client has not set the rbr callback function
    row_events_count_ = binlog_rbr_fn != NULL ? row_events_count_ + 1 : 1;

    return rows_event;
}

void drizzle_binlog_rbr_st::add_table_row_mapping(
    drizzle_binlog_rows_event_st *rows_event)
{
    if (binlog_rbr_fn != NULL )
    {
        tableid_rows_events.add_mapping(rows_event);
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
        // delete created row event structs
        rows_events.clear();

        // delete created tablemap event structs
        for (auto kv : tablemap_events)
        {
            free(kv.second);
        }
        tablemap_events.clear();
    }

    // clear the mapping between tables and rows events
    tableid_rows_events.reset();

    // clear the mapping between tablename and table id
    tablename_tableid.clear();

    rows_event_it.it = rows_events.end();
    row_events_count_ = 0;
    current_tablemap_id = 0;
    rows_events_parsed = 0;
    rows_event_it.reset();
} // drizzle_binlog_rbr_st::reset

size_t drizzle_binlog_rbr_st::get_row_events_count(const char *table_name)
{
    auto table_id = tableid_by_tablename(table_name);

    return tableid_rows_events.row_events_count(table_id);
}


size_t drizzle_binlog_rbr_row_events_count_(drizzle_binlog_rbr_st *binlog_rbr,
                                            ...)
{
    size_t rows_count = 0;
    const char *table_name = NULL;
    va_list args;

    va_start(args, binlog_rbr);
    table_name = va_arg(args, const char *);
    va_end(args);

    if (table_name == NULL)
    {
        rows_count = binlog_rbr->row_events_count_;
    }
    else
    {
        rows_count = binlog_rbr->get_row_events_count(table_name);
    }

    return rows_count;
}


uint64_t drizzle_binlog_rbr_xid(const drizzle_binlog_rbr_st *binlog_rbr)
{
    return binlog_rbr->xid_event.xid;
}


drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_next_(
    drizzle_binlog_rbr_st *binlog_rbr, ...)
{
    drizzle_binlog_rows_event_st *rows_event = NULL;
    const char *table_name = NULL;

    va_list args;

    va_start(args, binlog_rbr);
    table_name = va_arg(args, const char *);
    va_end(args);

    // get the next event for a specific table
    if (table_name != NULL)
    {
        auto schema_table = binlog_rbr->tableid_by_tablename(table_name);
        return binlog_rbr->tableid_rows_events.next_row_event(schema_table);
    }

    if (!binlog_rbr->rows_event_it.active)
    {
        binlog_rbr->rows_event_it.it = binlog_rbr->rows_events.begin();
        binlog_rbr->rows_event_it.active = true;
    }
    else if (binlog_rbr->rows_event_it.it == binlog_rbr->rows_events.end())
    {
        return NULL;
    }

    rows_event = *binlog_rbr->rows_event_it.it;
    binlog_rbr->rows_event_it.it++;
    return rows_event;
} // drizzle_binlog_rbr_rows_event_next

drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_prev_(
    drizzle_binlog_rbr_st *binlog_rbr, ...)
{
    if (prev(binlog_rbr->rows_event_it.it) <=
        binlog_rbr->rows_events.begin() - 1)
    {
        binlog_rbr->rows_event_it.it = binlog_rbr->rows_events.begin() - 1;
        return NULL;
    }

    binlog_rbr->rows_event_it.it--;
    return *binlog_rbr->rows_event_it.it;
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


drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_tablemap_by_tablename(
    drizzle_binlog_rbr_st *binlog_rbr, const char *table_name)
{
    if (table_name == NULL)
    {
        return NULL;
    }

    return binlog_rbr->get_tablemap_event(table_name);
}

drizzle_return_t drizzle_binlog_rbr_row_events_seek_(
    drizzle_binlog_rbr_st *binlog_rbr,
    drizzle_list_position_t pos,
    ...)
{
    const char *table_name = NULL;
    va_list args;

    va_start(args, pos);
    table_name = va_arg(args, const char *);
    va_end(args);

    if (table_name != NULL)
    {
        if (auto table_id = binlog_rbr->tableid_by_tablename(table_name) > 0)
        {
            binlog_rbr->tableid_rows_events.set_rows_events_it(table_id, pos);
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
} // drizzle_binlog_rbr_row_events_seek_


drizzle_return_t drizzle_binlog_rbr_change_db(drizzle_binlog_rbr_st *binlog_rbr,
                                              const char *db)
{
    if (db == NULL || binlog_rbr == NULL)
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }
    strncpy(binlog_rbr->db, db, DRIZZLE_MAX_DB_SIZE);
    binlog_rbr->db[DRIZZLE_MAX_DB_SIZE - 1] = 0;
    return DRIZZLE_RETURN_OK;
}

const char *drizzle_binlog_rbr_db(const drizzle_binlog_rbr_st *binlog_rbr)
{
    if (binlog_rbr == NULL)
    {
        return NULL;
    }
    return binlog_rbr->db;
}

drizzle_binlog_row_st *drizzle_binlog_rbr_get_row(
    drizzle_binlog_rows_event_st *rows_event)
{
    drizzle_binlog_row_st *row = NULL;

    if (rows_event->current_row == 0)
    {
        row = &rows_event->rows.at(rows_event->current_row++);
    }
    else if (++rows_event->current_row < rows_event->rows.size())
    {
        row = &rows_event->rows.at(rows_event->current_row);
    }
    return row;
}

drizzle_return_t drizzle_binlog_field_info(drizzle_binlog_row_st *row,
                                           size_t field_idx,
                                           drizzle_column_type_t *type,
                                           drizzle_field_datatype_t *datatype,
                                           bool *is_unsigned)
{
    if (field_idx >= row->values_before.size())
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_binlog_column_value_st *column_value = &row->values_before.at(
            field_idx);
    *type = column_value->type;
    *datatype =  get_field_datatype(column_value->type);
    *is_unsigned = column_value->is_unsigned;
    return DRIZZLE_RETURN_OK;
}


/**
 * SQL to query information_schema.columns
 */
#define INFORMATION_SCHEMA_QUERY \
    "SELECT " \
    "C.table_schema, " \
    "C.table_name, "\
    "C.column_name, "\
    "ABS(C.ordinal_position - 1) ordinal_position, "\
    "IF(C.column_type REGEXP 'unsigned', "\
    "   TRUE, "\
    "   FALSE) is_unsigned,  "\
    "IF(C.is_nullable REGEXP 'YES', "\
    "   TRUE, "\
    "   FALSE) is_nullable "\
    "FROM COLUMNS C "\
    "WHERE find_in_set(C.table_schema, "\
    "                  'information_schema,sys,mysql,performance_schema') < 1 "\
    "GROUP BY C.table_schema, "\
    "C.table_name, "\
    "C.column_name, "\
    "ordinal_position, "\
    "is_unsigned "\
    "ORDER BY C.table_schema, "\
    "C.table_name, "\
    "C.column_name, "\
    "ordinal_position"

db_information_schema_columns_st *drizzle_information_schema_create(
    drizzle_st *con)
{
    if (con == NULL)
    {
        return NULL;
    }

    auto *information_schema =
        new (std::nothrow) db_information_schema_columns_st();
    if (information_schema == NULL)
    {
        return NULL;
    }
    drizzle_return_t ret;

    char orig_schema[DRIZZLE_MAX_DB_SIZE];
    sprintf(orig_schema, "%s", drizzle_db(con));

    ret = drizzle_select_db(con, "information_schema");
    if (drizzle_failed(ret))
    {
        drizzle_set_error(con, __FILE_LINE_FUNC__,
                          "Could not select db `INFORMATION_SCHEMA`");
        return NULL;
    }

    drizzle_result_st *result = drizzle_query(con, INFORMATION_SCHEMA_QUERY, 0,
                                              &ret);

    if (ret != DRIZZLE_RETURN_OK)
    {
        drizzle_set_error(con, __FILE_LINE_FUNC__,
                          "Could not query db `INFORMATION_SCHEMA.COLUMNS`\n");
        return NULL;
    }

    if (drizzle_failed(drizzle_result_buffer(result)))
    {
        drizzle_set_error(con, __FILE_LINE_FUNC__, "Could not buffer result");
        return NULL;
    }

    drizzle_row_t row;
    while ((row = drizzle_row_next(result)))
    {
        information_schema->add(
            row[0], row[1], row[2],
            (size_t) atoi(row[3]), atoi(row[4]),
            atoi(row[5]));
    }

    drizzle_select_db(con, orig_schema);
    return information_schema;
} // drizzle_information_schema_create
