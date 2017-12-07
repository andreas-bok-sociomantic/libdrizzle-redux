#pragma once

#include <type_traits>
#include <typeinfo>

/**
 * @brief      Gets the column value struct at a specified index
 *
 *             Checks that all arguments are valid.
 *
 * @param      row           The drizzle binlog row struct
 * @param[in]  field_number  The field number
 * @param      before        The variable to save before value into
 * @param      after         The variable to save after value into
 * @param[in]  ret_ptr       The ret pointer
 *
 * @tparam     T             The datatype of the field to get
 *
 * @return     Pointer to a column value struct or NULL if arguments are not
 *             valid
 */
template<typename T>
drizzle_binlog_column_value_st *get_column_value_st(drizzle_binlog_row_st *row,
                                                    size_t field_number,
                                                    const T *before,
                                                    const T *after,
                                                    drizzle_return_t ret_ptr);

/**
 * @brief      Assigns the value of a column
 *
 * @param      column_value  The column value struct
 * @param      dest          The destination
 * @param[in]  is_unsigned   Indicates if unsigned
 *
 * @tparam     T             The type
 *
 * @return     DRIZZLE_RETURN_OK if the field could be assigned,
 *             DRIZZLE_RETURN_INVALID_ARGUMENT or information about the
 *             assignment error
 */
template<typename T>
drizzle_return_t assign_field_value(
    drizzle_binlog_column_value_st *column_value,
    T *dest, bool is_unsigned);

/**
 * @brief      Gets the value of a column
 *
 * @param      row           Pointer to drizzle binlog row struct
 * @param      column_value  Pointer to column value struct
 * @param[in]  field_number  The field number
 * @param      before        The variable to save before value into
 * @param      after         The variable to save after value into
 * @param[in]  type1         The original column type
 * @param[in]  type2         The expected column type
 * @param[in]  is_unsigned   Indicates if unsigned
 *
 * @tparam     T             The datatype of the field to assign
 * @tparam     U             The enum type to do check the column type
 *
 * @return     DRIZZLE_RETURN_OK if the field could be assigned,
 *             DRIZZLE_RETURN_INVALID_ARGUMENT or information about the
 *             assignment error
 */
template<typename T, typename U>
drizzle_return_t drizzle_binlog_get_field_value(drizzle_binlog_row_st *row,
                                                drizzle_binlog_column_value_st *column_value,
                                                size_t field_number, T *before, T *after,
                                                U type1, U type2, bool is_unsigned=true);

// check if the value is signed/unsigned
// make a double cast through the actual type and the longest type
//

/**
 * @brief      Casts raw bytes to another datatype
 *             First casts the value to the original datatype specified in the
 *             table definition, then casts it to the datatype of the output
 * value
 *
 * @param      dest         The variable to assign
 * @param[in]  src          The raw bytes to cast
 * @param[in]  is_unsigned  Indicates if the value is signed or unsigned
 *
 * @tparam     T_UNSIGNED   The unsigned datatype to cast to
 * @tparam     T_SIGNED     The signed datatype to cast to
 * @tparam     TYPE_DEST    The type of the variable to assign
 */
template <typename T_UNSIGNED, typename T_SIGNED, typename TYPE_DEST>
void signedness_cast(TYPE_DEST *dest, const unsigned char *src,
                     bool is_unsigned);

/*template<typename T>
drizzle_return_t drizzle_binlog_get_integer(drizzle_binlog_row_st *row,
                                        size_t field_number, T *before,
                                        T *after, bool is_unsigned);
*/
