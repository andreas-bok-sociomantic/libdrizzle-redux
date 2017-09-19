#pragma once

#include <unordered_map>
#include <vector>
#include <iterator>

struct drizzle_binlog_rbr_st
{

    typedef std::unordered_map<uint64_t, drizzle_binlog_tablemap_event_st*>
        map_tablemap_events;
    typedef std::vector<drizzle_binlog_rows_event_st** > vec_ptr_row_events;
    typedef std::unordered_map<const char*, vec_ptr_row_events>
        map_tablename_vec_row_events_ptr;
    typedef std::vector<drizzle_binlog_rows_event_st*> vec_row_events;

    struct tablename_rows_events_iterator
    {
        bool active;
        uint64_t table_id;
        char table_name[DRIZZLE_MAX_TABLE_SIZE];
        map_tablename_vec_row_events_ptr::iterator it;

        tablename_rows_events_iterator() :
            active(false),
            table_id(0)
        {
        }

        void reset()
        {
            active = false;
        }

        map_tablename_vec_row_events_ptr::iterator curr()
        {
            return it;
        }
    };

    struct rows_events_iterator
    {
        bool active;
        vec_row_events::iterator it;
        rows_events_iterator() :
        active(false)
        {}

        void reset()
        {
            active = false;
        }

        vec_row_events::iterator curr()
        {
            return it;
        }

        void operator++()
        {
            it++;
        }

        void operator--(int)
        {
            it--;
        }
    };

    // pointer to binlog struct
    drizzle_binlog_st *binlog;

    // callback function
    drizzle_binlog_rbr_fn *binlog_rbr_fn;


    drizzle_binlog_xid_event_st xid_event;
    drizzle_binlog_query_event_st query_event;

    map_tablemap_events tablemap_events;
    map_tablename_vec_row_events_ptr map_tablename_rows_events;

    vec_row_events rows_events;
    rows_events_iterator rows_event_it;
    size_t row_events_count_;
    size_t rows_events_parsed;
    uint64_t current_tablemap_id;

    tablename_rows_events_iterator tablename_rows_events_it;

    drizzle_binlog_rbr_st() :
        binlog(NULL),
        binlog_rbr_fn(NULL),
        row_events_count_(0),
        current_tablemap_id(0)

    {
        tablename_rows_events_it.it = map_tablename_rows_events.end();
    }

    ~drizzle_binlog_rbr_st()
    {

    }

    void reset(bool free_rows=false);


    /**
     * Gets a tablemap event.
     *
     *             Get either the table map event just parsed or by table id
     *
     * @param[in]  table_id  The table identifier
     *
     * @return     Pointer to a tablemap event struct
     */
    drizzle_binlog_tablemap_event_st *get_tablemap_event(uint64_t table_id=0);

    /**
     * @brief      Creates a tablemap event.
     *
     *             Create and return a new tablemap event or an existing one if
     *             an event with the same table id already exists
     *
     * @param[in]  table_id  The table identifier
     *
     * @return     Pointer to a tablemap event struct
     */
    drizzle_binlog_tablemap_event_st *create_tablemap_event(uint64_t table_id);


    /**
     * Get a rows event
     * If the current number of rows exceeds the size of the vector a new
     * row event struct is created.
     * Else the next unused row struct is returned
     *
     * @return     Pointer a rows event struct
     */
    drizzle_binlog_rows_event_st *get_rows_event();

    /**
     * @brief      Get a rows event.
     *
     *             If the number of rows events is less than the number of
     *             available events an unused event is returned otherwise a
     *             new rows event is created
     *
     * @return     Pointer to a rows event
     */
    drizzle_binlog_rows_event_st *create_rows_event();


    /**
     * @brief      Adds a table row mapping.
     *
     * @param      event  The event
     */
    void add_table_row_mapping(drizzle_binlog_rows_event_st *event);

    /**
     * Adds a drizzle binlog event
     *
     * @param      binlog_event  a drizzle binlog event struct
     */
    void add_binlog_event(drizzle_binlog_event_st* binlog_event);
};
