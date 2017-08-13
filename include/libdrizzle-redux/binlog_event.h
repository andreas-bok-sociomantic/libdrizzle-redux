#pragma once

#include <inttypes.h>
#include <memory>

struct drizzle_binlog_event_header_st
{
protected:
    uint32_t _timestamp;
    drizzle_binlog_event_types_t _type;
    uint32_t _server_id;
    uint32_t _length;
    uint32_t _next_pos;
    uint16_t _header_flags;
    uint16_t _extra_flags;
    uint32_t _checksum;

public:
    drizzle_binlog_event_header_st(drizzle_binlog_event_st *event=NULL);
    ~drizzle_binlog_event_header_st();

    DRIZZLE_API
    uint32_t timestamp();

    DRIZZLE_API
    drizzle_binlog_event_types_t type();

    DRIZZLE_API
    uint32_t server_id();

    DRIZZLE_API
    uint32_t length();

    DRIZZLE_API
    uint32_t next_pos();

    DRIZZLE_API
    uint16_t header_flags();

    DRIZZLE_API
    uint16_t extra_flags();

    DRIZZLE_API
    uint32_t checksum();

// private:
//     struct binlog_event_header_impl;
//     std::unique_ptr<binlog_event_header_impl> _impl;
};

/**
 * @brief      XID event
 */
struct drizzle_binlog_xid_event_st : public drizzle_binlog_event_header_st
{
public:
    /**
     * @brief      Constructor
     */
    drizzle_binlog_xid_event_st(drizzle_binlog_event_st *event=NULL);

    /**
     * @brief      Destroys the object.
     */
    ~drizzle_binlog_xid_event_st();

    /**
     * @brief      parse event specific part of the binlog event
     *
     * @param      event  a drizzle_binlog_event_st
     */
    DRIZZLE_API
    void parse(drizzle_binlog_event_st *event);

    /**
     * Inherited methods from drizzle_binlog_event_header_st
     */
    using drizzle_binlog_event_header_st::timestamp;
    using drizzle_binlog_event_header_st::type;
    using drizzle_binlog_event_header_st::server_id;
    using drizzle_binlog_event_header_st::length;
    using drizzle_binlog_event_header_st::next_pos;
    using drizzle_binlog_event_header_st::header_flags;
    using drizzle_binlog_event_header_st::extra_flags;
    using drizzle_binlog_event_header_st::checksum;

    /**
     * @brief      Get the xid id for the transaction
     *
     * @return     Return the xid for the transaction
     */
    DRIZZLE_API
    uint64_t xid();

    friend std::ostream &operator<<(std::ostream & _stream, drizzle_binlog_xid_event_st const &e);

    friend drizzle_binlog_event_header_st;
private:
    /**
     * @brief      Struct hiding implementation
     */
    struct xid_event_impl;

    /**
     * Pointer to struct with implementation details
     */
    std::unique_ptr<xid_event_impl> _impl;
};


/**
 * @brief      Binlog QUERY_EVENT
 */
struct drizzle_binlog_query_event_st : public drizzle_binlog_event_header_st
{
public:
    /**
     * @brief      Constructor
     */
    drizzle_binlog_query_event_st();

    /**
     * @brief      Destroys the object.
     */
    ~drizzle_binlog_query_event_st();

    /**
     * @brief      parse event specific part of the binlog event
     *
     * @param      event  a drizzle_binlog_event_st
     */
    void parse(drizzle_binlog_event_st *event);

    /**
     * Inherited methods from drizzle_binlog_event_header_st
     */
    using drizzle_binlog_event_header_st::timestamp;
    using drizzle_binlog_event_header_st::type;
    using drizzle_binlog_event_header_st::server_id;
    using drizzle_binlog_event_header_st::length;
    using drizzle_binlog_event_header_st::next_pos;
    using drizzle_binlog_event_header_st::header_flags;
    using drizzle_binlog_event_header_st::extra_flags;
    using drizzle_binlog_event_header_st::checksum;

    /**
     * @brief      { function_description }
     *
     * @return     { description_of_the_return_value }
     */
    DRIZZLE_API
    uint32_t slave_proxy_id();

    /**
     * @brief      { item_description }
     *
     * @return     Time in seconds
     */
    DRIZZLE_API
    uint32_t execution_time();

    /**
     * @brief      The error code resulting from execution of the statement on
     *             the master
     *
     * @return     MySQL error code
     */
    DRIZZLE_API
    uint16_t error_code();

    /**
     * @brief      The size of the status variable block in bytes
     *
     * @return     size of the status variables in bytes
     */
    DRIZZLE_API
    uint16_t status_vars_length();

    /**
     * @brief      Zero or more status variables. Each status variable consists
     *             of one byte code identifying the variable stored, followed by
     *             the value of the variable.
     *
     * @return     array of status vars
     */
    DRIZZLE_API
    uint8_t *status_vars();

    /**
     * @brief      The name of the schema which casused the binlog event
     *
     * @return     { description_of_the_return_value }
     */
    DRIZZLE_API
    unsigned char *schema();

    /**
     * @brief      The SQL statement which was executed
     *
     * @return     SQL query string
     */
    DRIZZLE_API
    unsigned char *query();

private:
    /**
     * @brief      Struct hiding implementation
     */
    struct query_event_impl;

    /**
     * Pointer to struct with implementation details
     */
    std::unique_ptr<query_event_impl> _impl;
};


/**
 * @brief          TABLE_MAP_EVENT
 *
 * @description    Event used in Row Based Replication with definitions of db
 *                 tables used
 */
struct drizzle_binlog_tablemap_event_st : public drizzle_binlog_event_header_st
{
public:
    /**
     * @brief      Constructor
     */
    drizzle_binlog_tablemap_event_st();

    /**
     * @brief      Destroys the object.
     */
    ~drizzle_binlog_tablemap_event_st();

    /**
     * @brief      parse event specific part of the binlog event
     *
     * @param      event  a drizzle_binlog_event_st
     */
    void parse(drizzle_binlog_event_st *event);

    /**
     * Inherited methods from drizzle_binlog_event_header_st
     */
    using drizzle_binlog_event_header_st::timestamp;
    using drizzle_binlog_event_header_st::type;
    using drizzle_binlog_event_header_st::server_id;
    using drizzle_binlog_event_header_st::length;
    using drizzle_binlog_event_header_st::next_pos;
    using drizzle_binlog_event_header_st::header_flags;
    using drizzle_binlog_event_header_st::extra_flags;
    using drizzle_binlog_event_header_st::checksum;

    /**
     * @brief      Unique id assigned to a table by the MySQL server
     *
     * @return     a table id
     */
    DRIZZLE_API
    uint64_t table_id();

    /**
     * @brief      A database schema name
     *
     * @return     database name
     */
    DRIZZLE_API
    unsigned char *schema_name();

    /**
     * @brief      A database table name
     *
     * @return     table name
     */
    DRIZZLE_API
    unsigned char *table_name();

    /**
     * @brief      The number of columns in the table
     *
     * @return     number of columns
     */
    DRIZZLE_API
    uint64_t column_count();

    /**
     * @brief      An array of column types, one byte per column.
     *             To find the meanings of these values, look at
     *             enum_field_types in the mysql_com.h header file.
     *
     * @return     an array of column types
     */
    DRIZZLE_API
    uint8_t *column_type_def();

    /**
     * @brief      Metadata for table columns
     *
     *             \see log_event.h in the MySQL source code for contents and
     *             format.
     *
     * @return     array of metadata
     */
    DRIZZLE_API
    uint8_t *field_metadata();

    /**
     * @brief      Length of the field_metadata array.
     *
     * @return     Size of the field_metadata array in bytes
     */
    DRIZZLE_API
    uint64_t field_metadata_len();

    /**
     * @brief      Bit-field indicating whether each column can be NULL, one bit
     *             per column
     *
     * @return     A bitmap
     */
    DRIZZLE_API
    uint8_t *null_bitmap();

private:
    /**
     * @brief      Struct hiding implementation
     */
    struct tablemap_event_impl;

    /**
     * Pointer to struct with implementation details
     */
    std::unique_ptr<tablemap_event_impl> _impl;
};


/**
 * @brief          Rows Event
 *
 * @description    Implementation of rows events version 4 which is written by
 *                 MySQL 5.0 and later
 *
 * Three basic kinds of ROWS_EVENT exist:
 * +=========================================================================+
 * | Event             | SQL Cmd | Contents                                  |
 * +-------------------+---------+-------------------------------------------+
 * | WRITE_ROWS_EVENT  | INSERT  | the row data to insert                    |
 * +-------------------+---------+-------------------------------------------+
 * | DELETE_ROWS_EVENT | DELETE  | as much data as needed to identify a row  |
 * +-------------------+---------+-------------------------------------------+
 * | UPDATE_ROWS_EVENT | UPDATE  | as much data as needed to identify a row  |
 * |                   |         | + the data to change                      |
 * +=========================================================================+
 */
struct drizzle_binlog_rows_event_st : public drizzle_binlog_event_header_st
{
public:
    /**
     * @brief      Constructor
     */
    drizzle_binlog_rows_event_st();

    /**
     * @brief      Destroys the object.
     */
    ~drizzle_binlog_rows_event_st();

    /**
     * Parse event specific data for rows events.
     * A table map event is required to get type definitions and
     * metatadata for columns
     *
     * @param      event            initialized drizzle_binlog_event
     *                              structure
     * @param[in]  table_map_event  A table_map_event structure
     */
    DRIZZLE_API
    void parse(drizzle_binlog_event_st *event,
               drizzle_binlog_tablemap_event_st *table_map_event);


    /**
     * Inherited methods from drizzle_binlog_event_header_st
     */
    using drizzle_binlog_event_header_st::timestamp;
    using drizzle_binlog_event_header_st::type;
    using drizzle_binlog_event_header_st::server_id;
    using drizzle_binlog_event_header_st::length;
    using drizzle_binlog_event_header_st::next_pos;
    using drizzle_binlog_event_header_st::header_flags;
    using drizzle_binlog_event_header_st::extra_flags;
    using drizzle_binlog_event_header_st::checksum;


    /**
     * @brief      The id of the table of assigned by the MySQL
     *             server
     *
     * @return     A table id
     */
    DRIZZLE_API
    uint64_t table_id();

    /**
     * @brief      The number of columns in the associated table
     *
     * @return     Number of columns
     */
    DRIZZLE_API
    uint64_t column_count();

private:
    /**
     * @brief      Struct hiding implementation
     */
    struct rows_event_impl;

    /**
     * Pointer to struct with implementation details
     */
    std::unique_ptr<rows_event_impl> _impl;
};


/**
 * @brief      Get a xid event
 *
 * @param      event drizzle_binlog_event_st
 *
 * @return     a drizzle_binlog_xid_event_st
 */
DRIZZLE_API
drizzle_binlog_xid_event_st *drizzle_binlog_get_xid_event(
    drizzle_binlog_event_st *event);


/**
 * @brief      Get a query event
 *
 * @param      event  a drizzle_binlog_event_st
 *
 * @return     a drizzle_binlog_query_event_st
 */
DRIZZLE_API
drizzle_binlog_query_event_st *drizzle_binlog_get_query_event(
    drizzle_binlog_event_st *event);


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
 * @brief      Get a binlog rows event
 *
 * @param      event            A drizzle_binlog_event_st
 * @param      table_map_event  Pointer to a table_map_event
 *
 * @return     Return a drizzle_binlog_rows_event_st
 */
DRIZZLE_API
drizzle_binlog_rows_event_st *drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event,
    drizzle_binlog_tablemap_event_st *table_map_event);


/**
 * @brief      Get the XID event from a binlog event group
 *
 * @return     Pointer to a drizzle_binlog_xid_event struct
 */
DRIZZLE_API
drizzle_binlog_xid_event_st *drizzle_binlog_rbr_get_xid_event(
    drizzle_binlog_rbr_st* rbr);

/**
 * @brief      Get the Query event from a binlog event group
 *
 * @return     Pointer to a drizzle_binlog_query_event
 */
DRIZZLE_API
drizzle_binlog_query_event_st *drizzle_binlog_rbr_get_query_event(
    drizzle_binlog_rbr_st* rbr);

/**
 * @brief      Set the client callback function to receive row based
 *             replication event group notifications
 *
 * @param      binlog_st  An initialized drizzle_binlog structure
 * @param      rbr_fn     A callback function
 */
DRIZZLE_API
/**
 * @brief      Set the client callback function to receive row based
 *             replication event group notifications
 *
 * @param      binlog_st  An initialized binlog structure
 * @param      rbr_fn     The function callback defined in (drizzle_binlog_rbr_fn)()
 * @param      context    A pointer to user data which will be used for the
 *                        callback functions
 */
drizzle_return_t drizzle_binlog_set_rbr_fn(drizzle_binlog_st *binlog,
    drizzle_binlog_rbr_fn *rbr_fn, void *context);
