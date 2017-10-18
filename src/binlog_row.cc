#include "config.h"
#include "src/common.h"

template<typename T>
drizzle_return_t drizzle_binlog_get_field_value(
    drizzle_binlog_column_value_st *column_value,
    T *val)
{
    if (column_value == NULL || val == NULL )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_return_t ret = DRIZZLE_RETURN_OK;

    switch (column_value->type)
    {
        case DRIZZLE_COLUMN_TYPE_NULL:
            ret = DRIZZLE_RETURN_NULL_SIZE;
            break;
        case DRIZZLE_COLUMN_TYPE_TINY:

            *val = (uint32_t) (*(uint8_t *) column_value->raw_value);
            break;
        case DRIZZLE_COLUMN_TYPE_SHORT:
        case DRIZZLE_COLUMN_TYPE_YEAR:
            *val = (uint32_t) (*(uint16_t *) column_value->raw_value);
            break;
        case DRIZZLE_COLUMN_TYPE_INT24:
        case DRIZZLE_COLUMN_TYPE_LONG:
            *val = (uint32_t) (*(uint32_t *) column_value->raw_value);
            break;
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
            *val = (uint32_t) (*(uint64_t *) column_value->raw_value);
            if (*val > UINT32_MAX)
            {
                ret = DRIZZLE_RETURN_TRUNCATED;
            }
            break;
        case DRIZZLE_COLUMN_TYPE_FLOAT:
            ret = DRIZZLE_RETURN_TRUNCATED;
            *val = (uint32_t) (*(float *) column_value->raw_value);
            break;
        case DRIZZLE_COLUMN_TYPE_DOUBLE:
            ret = DRIZZLE_RETURN_TRUNCATED;
            *val = (uint32_t) (*(double *) column_value->raw_value);
            break;
        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_DATETIME:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
        case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
        case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
        case DRIZZLE_COLUMN_TYPE_BLOB:
        case DRIZZLE_COLUMN_TYPE_BIT:
        case DRIZZLE_COLUMN_TYPE_STRING:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
        case DRIZZLE_COLUMN_TYPE_DECIMAL:
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
        case DRIZZLE_COLUMN_TYPE_NEWDATE:
        case DRIZZLE_COLUMN_TYPE_VARCHAR:
        case DRIZZLE_COLUMN_TYPE_ENUM:
        case DRIZZLE_COLUMN_TYPE_SET:
        case DRIZZLE_COLUMN_TYPE_GEOMETRY:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
        case DRIZZLE_COLUMN_TYPE_DATETIME2:
        case DRIZZLE_COLUMN_TYPE_TIME2:
        default:
            ret = DRIZZLE_RETURN_INVALID_CONVERSION;
            *val = 0;
    } // switch

    return ret;
} // drizzle_binlog_get_field_value


drizzle_return_t drizzle_binlog_get_int(drizzle_binlog_row_st *row,
                                        size_t field_number, uint32_t *before,
                                        uint32_t *after)
{
    if (row == NULL || field_number >= row->values_before.size() ||
        before == NULL || after == NULL )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_binlog_column_value_st *column_value = &row->values_before.at(
            field_number);

    if ( column_protocol_datatype(column_value->type) != NUMERICAL)
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }


    drizzle_return_t ret_before = DRIZZLE_RETURN_OK;
    drizzle_return_t ret_after = DRIZZLE_RETURN_OK;
    ret_before = drizzle_binlog_get_field_value(column_value, before);

    if (row->is_update_event)
    {
        column_value = &row->values_after.at(field_number);
        ret_after = drizzle_binlog_get_field_value(column_value, after);
    }

    return ret_before == DRIZZLE_RETURN_OK &&
           ret_after == DRIZZLE_RETURN_OK ? DRIZZLE_RETURN_OK :
           ret_before;
} // drizzle_binlog_get_int

drizzle_return_t drizzle_binlog_get_string(drizzle_binlog_row_st *row,
                                           size_t field_number,
                                           const unsigned char *before,
                                           const unsigned char *after)
{
    if (row == NULL || field_number >= row->values_before.size() ||
        before == NULL || after == NULL )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_binlog_column_value_st *column_value = &row->values_before.at(
            field_number);

    auto type = column_value->type;
    if ( !(type == DRIZZLE_COLUMN_TYPE_STRING ||
           type == DRIZZLE_COLUMN_TYPE_VARCHAR ||
           type == DRIZZLE_COLUMN_TYPE_VAR_STRING) )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_return_t ret_before = DRIZZLE_RETURN_OK;
    drizzle_return_t ret_after = DRIZZLE_RETURN_OK;
    before = column_value->raw_value;
    // ret_before = drizzle_binlog_get_field_value(column_value, before);

    if (row->is_update_event)
    {
        column_value = &row->values_after.at(field_number);
        after = column_value->raw_value;
        // ret_after = drizzle_binlog_get_field_value(column_value, after);
    }

    return ret_before == DRIZZLE_RETURN_OK &&
           ret_after == DRIZZLE_RETURN_OK ? DRIZZLE_RETURN_OK :
           ret_before;
} // drizzle_binlog_get_string

drizzle_return_t drizzle_binlog_get_double(drizzle_binlog_row_st *row,
                                           size_t field_number, double *before,
                                           double *after)
{
    if (row == NULL || field_number >= row->values_before.size() ||
        before == NULL || after == NULL )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }


    drizzle_binlog_column_value_st *column_value = &row->values_before.at(
            field_number);


    auto type = column_value->type;
    if ( !(type == DRIZZLE_COLUMN_TYPE_NEWDECIMAL ||
           type == DRIZZLE_COLUMN_TYPE_DECIMAL ||
           type == DRIZZLE_COLUMN_TYPE_FLOAT ||
           type == DRIZZLE_COLUMN_TYPE_DOUBLE) )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_return_t ret_before = DRIZZLE_RETURN_OK;
    drizzle_return_t ret_after = DRIZZLE_RETURN_OK;

    *before = (double) drizzle_get_byte8(column_value->raw_value);

    if (row->is_update_event)
    {
        column_value = &row->values_after.at(field_number);
        *after = (double) drizzle_get_byte8(column_value->raw_value);
        // ret_after = drizzle_binlog_get_field_value(column_value, after);
    }

    return ret_before == DRIZZLE_RETURN_OK &&
           ret_after == DRIZZLE_RETURN_OK ? DRIZZLE_RETURN_OK :
           ret_before;;
}
