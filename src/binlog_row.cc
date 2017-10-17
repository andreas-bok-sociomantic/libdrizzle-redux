#include "config.h"
#include "src/common.h"

drizzle_return_t drizzle_binlog_get_int(drizzle_binlog_row_st *row,
    size_t field_number, uint32_t *before, uint32_t *after)
{
    if (row == NULL || field_number >= row->values_before.size() ||
        before == NULL || after == NULL )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_binlog_column_value_st *column_value = &row->values_before.at(field_number);
    *before = column_value->field._uint32;

    if (row->is_update_event)
    {
        column_value = &row->values_before.at(field_number);
        *after = column_value->field._uint32;
    }

    return DRIZZLE_RETURN_OK;
}
