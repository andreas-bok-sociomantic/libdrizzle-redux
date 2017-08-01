#pragma once

#include <inttypes.h>
#include <memory>

//#ifdef __cplusplus
struct drizzle_binlog_query_event_st;
struct drizzle_binlog_xid_event_st;
//#endif

/*#ifdef __cplusplus
extern "C" {
#endif*/

DRIZZLE_API
drizzle_binlog_xid_event_st *drizzle_binlog_get_xid_event( drizzle_binlog_event_st *event );

DRIZZLE_API
drizzle_binlog_query_event_st *drizzle_binlog_get_query_event( drizzle_binlog_event_st *event );



/*#ifdef __cplusplus
}
#endif*/


struct drizzle_binlog_xid_event_st
{
    uint64_t xid;
};


/* public.h */
class Book
{
    public:
  Book();
  ~Book();
  void print();
    private:
  class BookImpl;
  std::unique_ptr<BookImpl> _impl;
  BookImpl* const m_p;
};
