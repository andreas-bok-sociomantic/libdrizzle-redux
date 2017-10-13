#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief      Get a table map event
 *
 * @param      event  a drizzle_binlog_event_st
 *
 * @return     a drizzle_binlog_tablemap_event_st
 */
DRIZZLE_API
drizzle_binlog_tablemap_event_st *drizzle_binlog_get_tablemap_event(
    drizzle_binlog_event_st *event);


/**
 * @brief      Unique id assigned to a table by the MySQL server
 *
 * @return     a table id
 */
DRIZZLE_API
uint64_t drizzle_binlog_tablemap_event_table_id(
	drizzle_binlog_tablemap_event_st* event);

/**
 * @brief      A database schema name
 *
 * @return     database name
 */
DRIZZLE_API
const char* drizzle_binlog_tablemap_event_schema_name(
	drizzle_binlog_tablemap_event_st* event);

/**
 * @brief      A database table name
 *
 * @return     table name
 */
DRIZZLE_API
const char* drizzle_binlog_tablemap_event_table_name(
	drizzle_binlog_tablemap_event_st* event);

/**
 * @brief      The number of columns in the table
 *
 * @return     number of columns
 */
DRIZZLE_API
uint64_t drizzle_binlog_tablemap_event_column_count(
	drizzle_binlog_tablemap_event_st* event);

/**
 * @brief      An array of column types, one byte per column.
 *             To find the meanings of these values, look at
 *             enum_field_types in the mysql_com.h header file.
 *
 * @return     an array of column types
 */
/*DRIZZLE_API
drizzle_column_type_t* drizzle_binlog_tablemap_event_column_type_def(
	drizzle_binlog_tablemap_event_st* event);
*/

/**
 * Get the columntype by index
 *
 * @param      event         A tablemap event
 * @param[in]  column_index  The column index
 *
 * @return     a column type
 */
DRIZZLE_API
drizzle_column_type_t drizzle_binlog_tablemap_event_column_type(
    drizzle_binlog_tablemap_event_st* event, uint32_t column_index,
    drizzle_return_t* ret);


/**
 * @brief      Metadata for table columns
 *
 *             \see log_event.h in the MySQL source code for contents and
 *             format.
 *
 * @return     array of metadata
 */
DRIZZLE_API
uint8_t* drizzle_binlog_tablemap_event_field_metadata(
	drizzle_binlog_tablemap_event_st* event);

/**
 * @brief      Length of the field_metadata array.
 *
 * @return     Size of the field_metadata array in bytes
 */
DRIZZLE_API
uint64_t drizzle_binlog_tablemap_event_field_metadata_len(
	drizzle_binlog_tablemap_event_st* event);

/**
 * @brief      Bit-field indicating whether each column can be NULL, one bit
 *             per column
 *
 * @return     A bitmap
 */
DRIZZLE_API
uint8_t* drizzle_binlog_tablemap_event_null_bitmap(
	drizzle_binlog_tablemap_event_st* event);

#ifdef __cplusplus
}
#endif

