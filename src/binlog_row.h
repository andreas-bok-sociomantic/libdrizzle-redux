#pragma once

#include <type_traits>
#include <typeinfo>

template<typename T>
drizzle_return_t drizzle_binlog_get_field_value(drizzle_binlog_column_value_st *column_value,
    T *dest);


template<typename T>
drizzle_return_t drizzle_binlog_get_bigint_(drizzle_binlog_row_st *row,
                                        size_t field_number, T *before,
                                        T *after)
