#pragma once

#include <type_traits>
#include <typeinfo>

template<typename T>
drizzle_return_t drizzle_binlog_get_field_value(drizzle_binlog_column_value_st *column_value,
    T *dest);


/*template<typename T>
drizzle_return_t drizzle_binlog_get_integer(drizzle_binlog_row_st *row,
                                        size_t field_number, T *before,
                                        T *after);*/


// check if the value is signed/unsigned
// make a double cast through the actual type and the longest type
//
//
//
//

template <typename T_UNSIGNED, typename T_SIGNED, typename TYPE_DEST>
void signedness_cast(TYPE_DEST *dest, const unsigned char *src, bool is_unsigned);
