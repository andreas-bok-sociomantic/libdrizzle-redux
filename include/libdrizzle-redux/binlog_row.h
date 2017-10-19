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
    size_t field_number, uint32_t *before, uint32_t *after);

#ifdef __cplusplus
}
#endif
