#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief      Get an int field value
 *
 * @param      column  The column struct
 * @param      value   Pointer to a value
 *
 * @return     return value
 */
DRIZZLE_API
drizzle_return_t drizzle_binlog_get_int(drizzle_binlog_row_st *row,
                                        size_t field_number, int32_t *before,
                                        int32_t *after);

DRIZZLE_API
drizzle_return_t drizzle_binlog_get_uint(drizzle_binlog_row_st *row,
                                         size_t field_number, uint32_t *before,
                                         uint32_t *after);

DRIZZLE_API
drizzle_return_t drizzle_binlog_get_big_int(drizzle_binlog_row_st *row,
                                            size_t field_number,
                                            int64_t *before,
                                            int64_t *after);

DRIZZLE_API
drizzle_return_t drizzle_binlog_get_big_uint(drizzle_binlog_row_st *row,
                                             size_t field_number,
                                             uint64_t *before,
                                             uint64_t *after);


/**
 * @brief      Get a string field value
 *
 * @param      row           The row struct
 * @param[in]  field_number  The field number
 * @param      before        The before value
 * @param      after         The after value
 *
 * @return     DRIZZLE_RETURN_OK or DRIZZLE_RETURN_INVALID_ARGUMENT
 */
DRIZZLE_API
drizzle_return_t drizzle_binlog_get_string(drizzle_binlog_row_st *row,
                                           size_t field_number,
                                           const unsigned char **before,
                                           const unsigned char **after);

DRIZZLE_API
drizzle_return_t drizzle_binlog_get_double(drizzle_binlog_row_st *row,
                                           size_t field_number, double *before,
                                           double *after);


DRIZZLE_API
drizzle_return_t drizzle_binlog_get_timestamp(drizzle_binlog_row_st *row,
                                              size_t field_number,
                                              time_t *before,
                                              time_t *after);

#ifdef __cplusplus
}
#endif
