#include "config.h"
#include "src/common.h"

template<typename T>
drizzle_binlog_column_value_st *get_column_value_st(
    drizzle_binlog_row_st *row,
    size_t field_number,
    const T *before,
    const T *after,
    drizzle_return_t *ret_ptr)
{

    if (row == NULL || field_number >= row->values_before.size() ||
        before == NULL || after == NULL )
    {
        *ret_ptr = DRIZZLE_RETURN_INVALID_ARGUMENT;
        return NULL;
    }
    else
    {
        *ret_ptr = DRIZZLE_RETURN_OK;
        return &row->values_before.at(field_number);
    }
} // get_column_value_st

template <typename T_UNSIGNED, typename T_SIGNED, typename TYPE_DEST>
void signedness_cast(TYPE_DEST *dest, const unsigned char *src,
                     bool is_unsigned)
{
    if (is_unsigned)
    {
        *dest = (TYPE_DEST) (*(T_UNSIGNED *) src);
    }
    else
    {
        *dest = (TYPE_DEST) (*(T_SIGNED *) src);
    }
}


template<typename T>
drizzle_return_t assign_field_value(
    drizzle_binlog_column_value_st *column_value,
    T *val, bool is_unsigned)
{
    if (column_value == NULL || val == NULL )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_return_t ret = DRIZZLE_RETURN_OK;
    const unsigned char *src_value = column_value->raw_value;

    switch (column_value->type)
    {
        case DRIZZLE_COLUMN_TYPE_NULL:
            ret = DRIZZLE_RETURN_NULL_SIZE;
            break;
        case DRIZZLE_COLUMN_TYPE_TINY:
            /*signedness_cast<uint8_t, int8_t>(val, column_value->raw_value,
             *  column_value->is_unsigned);*/
            signedness_cast<uint8_t, int8_t>(val, src_value, is_unsigned);
            // *val = (uint32_t) (*(uint8_t *) column_value->raw_value);
            break;
        case DRIZZLE_COLUMN_TYPE_SHORT:
        case DRIZZLE_COLUMN_TYPE_YEAR:
            signedness_cast<uint16_t, int16_t>(val, src_value, is_unsigned);
            break;
        case DRIZZLE_COLUMN_TYPE_INT24:
        case DRIZZLE_COLUMN_TYPE_LONG:
            signedness_cast<uint32_t, int32_t>(val, src_value, is_unsigned);
            break;
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
            if (sizeof(T) <= sizeof(uint32_t))
            {
                signedness_cast<uint64_t, int64_t>(val, src_value, is_unsigned);
                if (is_unsigned)
                {
                    if ((uint32_t) *val > UINT32_MAX)
                    {
                        ret = DRIZZLE_RETURN_TRUNCATED;
                    }
                }
                else if ( *val > INT32_MAX )
                {
                    ret = DRIZZLE_RETURN_TRUNCATED;
                }

            }
            else if (sizeof(T) <= sizeof(uint64_t))
            {
                signedness_cast<uint64_t, int64_t>(val, src_value, is_unsigned);
            }
            else
            {
                // do something here
            }
            break;

        case DRIZZLE_COLUMN_TYPE_FLOAT:
        {
            // todo add support for truncation
            *val = (double) (*(float *) column_value->raw_value);
            break;
        }
        case DRIZZLE_COLUMN_TYPE_DOUBLE:
        {
            if (std::is_floating_point<typeof(T)>::value)
            {
                *val = (*(T *) column_value->raw_value);
            }
            else if (std::is_integral<T>::value)
            {
                if (sizeof(T) == sizeof(uint32_t))
                {
                    *val = (uint32_t) (*(T *) column_value->raw_value);
                }
                else if (sizeof(T) == sizeof(uint64_t))
                {
                    *val = (uint64_t) (*(T *) column_value->raw_value);
                }

                ret = DRIZZLE_RETURN_TRUNCATED;
            }
            break;
        }

        case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
        case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
        case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
        case DRIZZLE_COLUMN_TYPE_BLOB:
            break;
        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_DATETIME:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:

        case DRIZZLE_COLUMN_TYPE_BIT:
        case DRIZZLE_COLUMN_TYPE_STRING:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
        case DRIZZLE_COLUMN_TYPE_DECIMAL:
            ret = DRIZZLE_RETURN_INVALID_CONVERSION;
            break;
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
        {
            double d_value = 0.0;
            auto precision = column_value->metadata[0];
            auto decimals = column_value->metadata[1];
            unpackDecimalField(column_value->raw_value, precision, decimals,
                               &d_value);
            if (std::is_floating_point<typeof(T)>::value)
            {
                // *val = (*(T *) column_value->raw_value);
                *val = d_value;
            }
            else if (std::is_integral<T>::value)
            {
                if (sizeof(T) == sizeof(uint32_t))
                {
                    // *val = (uint32_t) (*(T *) column_value->raw_value);
                    // *val = (uint32_t) (*(T *) d_value);
                    // *val = (uint32_t) (*(T *) d_value);
                }
                else if (sizeof(T) == sizeof(uint64_t))
                {
                    // *val = (uint64_t) (*(T *) column_value->raw_value);
                    // *val = (uint64_t) (*(T *) &d_value);
                }

                ret = DRIZZLE_RETURN_TRUNCATED;
            }
            break;
        }
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
} // assign_field_value


template<typename T, typename U>
drizzle_return_t drizzle_binlog_get_field_value(drizzle_binlog_row_st *row,
                                                drizzle_binlog_column_value_st *column_value,
                                                size_t field_number, T *before, T *after,
                                                U type1, U type2, bool is_unsigned)
{
    if ( type1 != type2 )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_return_t ret_before = DRIZZLE_RETURN_OK;
    drizzle_return_t ret_after = DRIZZLE_RETURN_OK;
    ret_before = assign_field_value(column_value, before, is_unsigned);

    if (row->is_update_event)
    {
        column_value = &row->values_after.at(field_number);
        ret_after = assign_field_value(column_value, after, is_unsigned);
    }

    return ret_before == DRIZZLE_RETURN_OK &&
           ret_after == DRIZZLE_RETURN_OK ? DRIZZLE_RETURN_OK :
           ret_before;
}


/*drizzle_return_t drizzle_binlog_get_integer(drizzle_binlog_row_st *row,
                                        size_t field_number, T *before,
                                        T *after, bool is_unsigned)
{

}
*/
drizzle_return_t drizzle_binlog_get_int(drizzle_binlog_row_st *row,
                                        size_t field_number, int32_t *before,
                                        int32_t *after)
{
    drizzle_return_t ret;
    drizzle_binlog_column_value_st *column_value =
        get_column_value_st(row, field_number, before, after, &ret);

    if (ret != DRIZZLE_RETURN_OK)
    {
        return ret;
    }

    return drizzle_binlog_get_field_value(row, column_value, field_number,
                                          before, after,
                                          column_protocol_datatype(
                                              column_value->type), NUMERICAL,
                                          false);

} // drizzle_binlog_get_int
  //



drizzle_return_t drizzle_binlog_get_uint(drizzle_binlog_row_st *row,
                                         size_t field_number, uint32_t *before,
                                         uint32_t *after)
{
    drizzle_return_t ret;
    drizzle_binlog_column_value_st *column_value =
        get_column_value_st(row, field_number, before, after, &ret);

    if (ret != DRIZZLE_RETURN_OK)
    {
        return ret;
    }

    return drizzle_binlog_get_field_value(row, column_value, field_number,
                                          before, after,
                                          column_protocol_datatype(
                                              column_value->type), NUMERICAL,
                                          true);

} // drizzle_binlog_get_int


drizzle_return_t drizzle_binlog_get_big_int(drizzle_binlog_row_st *row,
                                            size_t field_number,
                                            int64_t *before,
                                            int64_t *after)
{
    drizzle_return_t ret;
    drizzle_binlog_column_value_st *column_value =
        get_column_value_st(row, field_number, before, after, &ret);

    if (ret != DRIZZLE_RETURN_OK)
    {
        return ret;
    }

    return drizzle_binlog_get_field_value(row, column_value, field_number,
                                          before, after,
                                          column_value->type,
                                          DRIZZLE_COLUMN_TYPE_LONGLONG, false);
} // drizzle_binlog_get_big_uint


drizzle_return_t drizzle_binlog_get_big_uint(drizzle_binlog_row_st *row,
                                             size_t field_number,
                                             uint64_t *before,
                                             uint64_t *after)
{
    drizzle_return_t ret;
    drizzle_binlog_column_value_st *column_value =
        get_column_value_st(row, field_number, before, after, &ret);

    if (ret != DRIZZLE_RETURN_OK)
    {
        return ret;
    }

    return drizzle_binlog_get_field_value(row, column_value, field_number,
                                          before, after,
                                          column_value->type,
                                          DRIZZLE_COLUMN_TYPE_LONGLONG, true);
} // drizzle_binlog_get_big_uint

drizzle_return_t drizzle_binlog_get_string(drizzle_binlog_row_st *row,
                                           size_t field_number,
                                           const unsigned char **before,
                                           const unsigned char **after)
{
    if (row == NULL || field_number >= row->values_before.size() ||
        before == NULL || after == NULL)
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_binlog_column_value_st *column_value = &row->values_before.at(
            field_number);

    if ( get_field_datatype(column_value->type) !=
         DRIZZLE_FIELD_DATATYPE_STRING )
    {
        return DRIZZLE_RETURN_INVALID_ARGUMENT;
    }

    drizzle_return_t ret_before = DRIZZLE_RETURN_OK;
    drizzle_return_t ret_after = DRIZZLE_RETURN_OK;
    *before = column_value->raw_value;
    // ret_before = assign_field_value(column_value, before);

    if (row->is_update_event)
    {
        column_value = &row->values_after.at(field_number);
        *after = column_value->raw_value;
        // ret_after = assign_field_value(column_value, after);
    }

    return ret_before == DRIZZLE_RETURN_OK &&
           ret_after == DRIZZLE_RETURN_OK ? DRIZZLE_RETURN_OK :
           ret_before;
} // drizzle_binlog_get_string

drizzle_return_t drizzle_binlog_get_double(drizzle_binlog_row_st *row,
                                           size_t field_number, double *before,
                                           double *after)
{
    drizzle_return_t ret;
    drizzle_binlog_column_value_st *column_value =
        get_column_value_st(row, field_number, before, after, &ret);

    if (ret != DRIZZLE_RETURN_OK)
    {
        return ret;
    }

    return drizzle_binlog_get_field_value(row, column_value, field_number,
                                          before, after,
                                          get_field_datatype(
                                              column_value->type),
                                          DRIZZLE_FIELD_DATATYPE_DECIMAL, false);
} // drizzle_binlog_get_double
