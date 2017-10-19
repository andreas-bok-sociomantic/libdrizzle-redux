#pragma once

#include <type_traits>
#include <typeinfo>

template<typename T>
drizzle_return_t drizzle_binlog_get_field_value(drizzle_binlog_column_value_st *column_value,
    T *dest);

