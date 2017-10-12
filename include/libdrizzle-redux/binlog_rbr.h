#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Get the number of row events in the binlog event group If a table
 *             name is passed as optional parameter the count for that specific
 *             table is returned
 *
 * @param      binlog_rbr  The binlog rbr
 * @param[in]  ...   optional table name
 *
 * @return     number of row events
 */
DRIZZLE_API
size_t drizzle_binlog_rbr_row_events_count_(drizzle_binlog_rbr_st *binlog_rbr,
                                           ...);
#define drizzle_binlog_rbr_row_events_count(binlog_rbr, ...) \
    drizzle_binlog_rbr_row_events_count_(binlog_rbr, ##__VA_ARGS__, NULL)


/**
 * @brief      Get the transaction id for the binlog event group
 *
 * @param[in]  a drizzle_binlog_rbr structure
 *
 * @return     id of the transaction
 */
DRIZZLE_API
uint64_t drizzle_binlog_rbr_xid(const drizzle_binlog_rbr_st *binlog_rbr);

/**
 * Read a row event from a binlog event group
 *
 * @param      binlog_rbr  A binlog rbr structure
 * @param      ret_ptr     Standard drizzle return value
 *
 * @return     a row event struct, or NULL if there are no more rows events
 */
DRIZZLE_API
drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_next_(
    drizzle_binlog_rbr_st *binlog_rbr, drizzle_return_t *ret_ptr,
    ...);
#define drizzle_binlog_rbr_rows_event_next(binlog_rbr, ret_ptr, ...) \
    drizzle_binlog_rbr_rows_event_next_(binlog_rbr, ret_ptr, ##__VA_ARGS__, NULL)


/**
 * @brief      Get the previous row event
 *
 * @param      binlog_rbr  a binlog rbr structure
 * @param      ret_ptr     Standard drizzle return value
 * @param[in]  <unnamed>   table name optional.
 *
 * @return     Pointer to a row event struct, or NULL if there are no more rows
 *             events
 */
DRIZZLE_API
drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_prev_(
    drizzle_binlog_rbr_st *binlog_rbr, drizzle_return_t *ret_ptr,
    ...);
#define drizzle_binlog_rbr_rows_event_prev(binlog_rbr, ret_ptr, ...) \
    drizzle_binlog_rbr_rows_event_prev_(binlog_rbr, ret_ptr, ##__VA_ARGS__, NULL)


DRIZZLE_API
drizzle_binlog_rows_event_st *drizzle_binlog_rbr_rows_event_index(
    drizzle_binlog_rbr_st *binlog_rbr, uint64_t row_event_idx);

/**
 * @brief      Get the zero based number of row which was most recently
 *             retrieved
 *
 * @param      binlog_rbr  The binlog rbr
 *
 * @return     the zero based row number, or -1 if the current row is out of
 *             bounds
 */
DRIZZLE_API
int64_t drizzle_binlog_rbr_rows_event_current(drizzle_binlog_rbr_st *binlog_rbr);



/**
 * @brief      Get a table map event by id
 *
 * @param      binlog_rbr  The binlog rbr
 * @param[in]  table_id    The table identifier
 *
 * @return     Pointer to a tablemap event struct or NULL if not found
 */
DRIZZLE_API
drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_tablemap_event(
    drizzle_binlog_rbr_st *binlog_rbr, uint64_t table_id);

/**
 * @brief      Get a table map event by table name and schema
 *
 * @param      binlog_rbr  A binlog rbr struct
 * @param[in]  table_name  The table name
 * @param[in]  <unnamed>   schema name, optional
 *
 * @return     Pointer to a tablemap event struct or NULL if not found
 */
DRIZZLE_API
drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_tablemap_by_tablename(
    drizzle_binlog_rbr_st *binlog_rbr, const char *table_name);

/**
 * @brief      Get a tablemap event from the associated rows event
 *
 * @param      event  a rows event
 *
 * @return     Pointer to a tablemap event struct or NULL if not found
 */
DRIZZLE_API
drizzle_binlog_tablemap_event_st *drizzle_binlog_rbr_rows_event_tablemap(
    drizzle_binlog_rbr_st *binlog_rbr, drizzle_binlog_rows_event_st *event);


/**
 * @brief      Seek to the beginning of a list of row events
 *
 * @param      binlog_rbr  binlog rbr struct
 * @param[in]  pos         The position to seek
 * @param[in]  <unnamed>   Optional table name
 *
 * @return     { description_of_the_return_value }
 */
DRIZZLE_API
drizzle_return_t drizzle_binlog_rbr_row_events_seek_(drizzle_binlog_rbr_st *binlog_rbr,
    drizzle_list_position_t pos, ...);
#define drizzle_binlog_rbr_row_events_seek(binlog_rbr, pos, ...) \
    drizzle_binlog_rbr_row_events_seek_(binlog_rbr, pos, ##__VA_ARGS__, NULL)


/**
 * @brief      Change the database schema used selecting binlog data
 *
 *             Tables are identified uniquely as <schema_name.table_name>.
 *             If tables with identical names exist in multiple schemas
 *
 * @todo Add more docs about why it is needed and when
 *
 * @param      binlog_rbr  The binlog rbr struct
 * @param[in]  db          The database name
 *
 * @return     { description_of_the_return_value }
 */
DRIZZLE_API
drizzle_return_t drizzle_binlog_rbr_change_db(drizzle_binlog_rbr_st *binlog_rbr, const char *db);


/**
 * @brief      Get schema used to identify table
 *
 * @param[in]  binlog_rbr  A binlog rbr struct
 *
 * @return     the schema name
 */
DRIZZLE_API
const char* drizzle_binlog_rbr_db(const drizzle_binlog_rbr_st *binlog_rbr);

#ifdef __cplusplus
}
#endif
