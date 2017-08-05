#pragma once

#include <inttypes.h>
#include <memory>

//#ifdef __cplusplus
// struct drizzle_binlog_query_event_st;
// struct drizzle_binlog_xid_event_st;
//#endif

/*#ifdef __cplusplus
extern "C" {
#endif*/


/*#ifdef __cplusplus
}
#endif*/


struct query_event_impl;
struct tablemap_event_impl;

struct drizzle_binlog_xid_event_st {
 public:
  drizzle_binlog_xid_event_st();
  ~drizzle_binlog_xid_event_st();
  DRIZZLE_API
  uint64_t xid();

 private:
    struct xid_event_impl;
    std::unique_ptr<xid_event_impl> _impl;
};

struct drizzle_binlog_query_event_st {
 public:
  drizzle_binlog_query_event_st();
  ~drizzle_binlog_query_event_st();

  DRIZZLE_API
  uint32_t slave_proxy_id();

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
  std::unique_ptr<query_event_impl> _impl;
};

struct drizzle_binlog_tablemap_event_st
{
    public :
    DRIZZLE_API
        drizzle_binlog_tablemap_event_st();
        DRIZZLE_API
        ~drizzle_binlog_tablemap_event_st();
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
        DRIZZLE_API
        unsigned char* null_bitmap();
    private:
        std::unique_ptr<tablemap_event_impl> _impl;
};

struct drizzle_binlog_rows_event_st
{
    public :
        drizzle_binlog_rows_event_st();
        ~drizzle_binlog_rows_event_st();
        uint64_t getMyInt();

    private :
        struct rows_event_impl;
        std::unique_ptr<rows_event_impl> _impl;
};


DRIZZLE_API
drizzle_binlog_xid_event_st *drizzle_binlog_get_xid_event(
    drizzle_binlog_event_st *event);

DRIZZLE_API
drizzle_binlog_query_event_st *drizzle_binlog_get_query_event(
    drizzle_binlog_event_st *event);

DRIZZLE_API
drizzle_binlog_tablemap_event_st* drizzle_binlog_get_tablemap_event(
    drizzle_binlog_event_st *event);

DRIZZLE_API
drizzle_binlog_rows_event_st* drizzle_binlog_get_rows_event(
    drizzle_binlog_event_st *event);
