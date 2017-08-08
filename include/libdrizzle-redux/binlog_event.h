#pragma once

#include <inttypes.h>
#include <memory>

// struct binlog_event_interface
// {
//   public:
//     DRIZZLE_API
//     uint32_t timestamp();

//     DRIZZLE_API
//     drizzle_binlog_event_types_t type();

//     DRIZZLE_API
//     uint32_t server_id();

//     DRIZZLE_API
//     uint32_t length();

//     DRIZZLE_API
//     uint32_t next_pos();

//     DRIZZLE_API
//     uint16_t flags();

//     DRIZZLE_API
//     uint16_t extra_flags();

//     DRIZZLE_API
//     uint32_t checksum();
// };


/**
 * @brief      XID event
 */
struct drizzle_binlog_xid_event_st {
public:
  /**
   * @brief      Constructor
   */
  drizzle_binlog_xid_event_st();

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
   * @brief      Get the xid id for the transaction
   *
   * @return     Return the xid for the transaction
   */
  DRIZZLE_API
  uint64_t xid();

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
struct drizzle_binlog_query_event_st {
 public:
  drizzle_binlog_query_event_st();
  ~drizzle_binlog_query_event_st();

  void parse(drizzle_binlog_event_st *event);

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

  DRIZZLE_API
  uint16_t error_code();

  DRIZZLE_API
  uint16_t status_vars_length();

  DRIZZLE_API
  unsigned char *status_vars();

  DRIZZLE_API
  unsigned char *schema();

  /**
   * @brief      Get the query string
   *
   * @return     { description_of_the_return_value }
   */
  DRIZZLE_API
  unsigned char *query();

 private:
  struct query_event_impl;
  std::unique_ptr<query_event_impl> _impl;
};

struct drizzle_binlog_tablemap_event_st
{
    public :
        /**
         * @brief      Constructor
         */
        drizzle_binlog_tablemap_event_st();

        /**
         * @brief      Destroys the object.
         */
        ~drizzle_binlog_tablemap_event_st();

        void parse(drizzle_binlog_event_st *event);
        DRIZZLE_API
        uint64_t table_id();
        DRIZZLE_API
        unsigned char* flags();
        DRIZZLE_API
        unsigned char* schema_name();
        DRIZZLE_API
        unsigned char* table_name();
        DRIZZLE_API
        uint64_t column_count();
        DRIZZLE_API
        unsigned char* column_type_def();
        DRIZZLE_API
        unsigned char* field_metadata();
        uint64_t field_metadata_len();
        DRIZZLE_API
        unsigned char* null_bitmap();

    private:
        struct tablemap_event_impl;
        std::unique_ptr<tablemap_event_impl> _impl;
};

/**
 * @brief Implementation of rows events version 4 which is written by MySQL 5.0
 * and later
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
struct drizzle_binlog_rows_event_st
{
    public :
        drizzle_binlog_rows_event_st();
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

    private :
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
drizzle_binlog_tablemap_event_st* drizzle_binlog_get_tablemap_event(
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
drizzle_binlog_rows_event_st* drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event,
    drizzle_binlog_tablemap_event_st *table_map_event);
