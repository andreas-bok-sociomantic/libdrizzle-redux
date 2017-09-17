#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the number of row events in the binlog event group
 *
 * @return     number of row events
 */
DRIZZLE_API
size_t drizzle_binlog_rbr_row_events_count(drizzle_binlog_rbr_st *binlog_rbr);

/**
 * Get the transaction id for the binlog event group
 *
 * @param      a drizzle_binlog_rbr structure
 *
 * @return     id of the transaction
 */
DRIZZLE_API
uint64_t drizzle_binlog_rbr_xid(drizzle_binlog_rbr_st *binlog_rbr);

/**
 * Read a row event from a binlog event group
 *
 * @param      binlog_rbr  A binlog rbr structure
 * @param      ret_ptr     Standard drizzle return value
 *
 * @return     a row event struct, or NULL if there are no more rows events
 */
DRIZZLE_API
drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_next(
    drizzle_binlog_rbr_st *binlog_rbr, drizzle_return_t *ret_ptr);


/**
 * Get the prev row event
 *
 * @param      binlog_rbr  a binlog rbr structure
 * @param      ret_ptr     Standard drizzle return value
 *
 * @return     { description_of_the_return_value }
 */
DRIZZLE_API
drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_prev(
    drizzle_binlog_rbr_st *binlog_rbr, drizzle_return_t *ret_ptr);

/**
 * @brief      Get a rows event by index
 *
 * @param      binlog_rbr     A binlog rbr struct
 * @param[in]  row_event_idx  An index
 *
 * @return     Pointer to a drizzle_binlog_rows_event struct or NULL if the
 *             index is out of bounds
 */
DRIZZLE_API
drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_index(
    drizzle_binlog_rbr_st *binlog_rbr, uint64_t row_event_idx);

/**
 * @brief      { function_description }
 *
 * @param      binlog_rbr  The binlog rbr
 *
 * @return     the zero based row number, or -1 if the current row is out of
 *             bounds
 */
DRIZZLE_API
int64_t drizzle_binlog_rbr_rows_event_current(drizzle_binlog_rbr_st *binlog_rbr);



/**
 * @brief      Get the next rows event from a table
 *
 * @param      binlog_rbr  A binlog_rbr struct
 * @param[in]  table_name  A table name
 *
 * @return     { description_of_the_return_value }
 */
drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_from_table_next(
    drizzle_binlog_rbr_st *binlog_rbr, const char* table_name);


DRIZZLE_API
/**
 * @brief      Get the number of rows events collected from a table
 *
 * @param      binlog_rbr  A binlog_rbr struct
 * @param[in]  table_name  A table name
 *
 * @return     -1 if the table was not found otherwise the number of rows events
 */
int64_t drizzle_binlog_rbr_rows_event_from_table_count(
    drizzle_binlog_rbr_st *binlog_rbr, const char* table_name);
#ifdef __cplusplus
}
#endif
