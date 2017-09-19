#pragma once

#include <unordered_map>
#include <vector>
#include <iterator>


struct tablename_rows_events_map
{
    typedef std::vector<drizzle_binlog_rows_event_st** > vec_ptr_row_events;
    typedef std::unordered_map<const char*, vec_ptr_row_events>
        map_tablename_vec_row_events_ptr;

    bool active;
    uint64_t table_id;
    char table_name[DRIZZLE_MAX_TABLE_SIZE];

    map_tablename_vec_row_events_ptr mapping;
    vec_ptr_row_events::iterator row_events_it;
    vec_ptr_row_events *curr_row_events;
    char table_name_curr[DRIZZLE_MAX_TABLE_SIZE];

    tablename_rows_events_map() :
        active(false),
        table_id(0)

    {
        table_name_curr[0] = '\0';
    }


    bool has_table(const char *tablename)
    {
        return mapping.find(tablename) != mapping.end();
    }

    bool set_rows_events_it(const char *tablename)
    {
        if (!has_table(tablename))
        {
            return false;
        }
        else
        {
            if (table_name_curr != tablename)
            {
                curr_row_events = &mapping.find(tablename)->second;
                row_events_it = curr_row_events->begin();
                strcpy(table_name_curr, tablename);
            }

            return true;
        }
    }

    drizzle_binlog_rows_event_st* next_row_event(const char *tablename)
    {
        if (!set_rows_events_it(tablename))
        {
            return NULL;
        }

        if (next(row_events_it) != curr_row_events->end())
        {
            return **(++row_events_it);
        }
        else
        {
            return NULL;
        }
    }

    void add_mapping(drizzle_binlog_rows_event_st *rows_event)
    {
        if (mapping.find(rows_event->table_name) == mapping.end())
        {
            vec_ptr_row_events vec;
            mapping.insert(std::make_pair(rows_event->table_name, vec));
        }

        auto vec_rows = &mapping.find(rows_event->table_name)->second;
        vec_rows->push_back(&rows_event);
    }

    void reset()
    {
        active = false;
        table_name_curr[0] = '\0';

        for (auto kv : mapping)
        {
            kv.second.clear();
        }
    }
};

struct drizzle_binlog_rbr_st
{
    typedef std::unordered_map<uint64_t, drizzle_binlog_tablemap_event_st*>
        map_tablemap_events;
    typedef std::vector<drizzle_binlog_rows_event_st** > vec_ptr_row_events;
    typedef std::unordered_map<const char*, vec_ptr_row_events>
        map_tablename_vec_row_events_ptr;
    typedef std::vector<drizzle_binlog_rows_event_st*> vec_row_events;

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
    tablename_rows_events_map tablename_rows_events;

    vec_row_events rows_events;
    rows_events_iterator rows_event_it;
    size_t row_events_count_;
    size_t rows_events_parsed;
    uint64_t current_tablemap_id;

    drizzle_binlog_rbr_st() :
        binlog(NULL),
        binlog_rbr_fn(NULL),
        row_events_count_(0),
        current_tablemap_id(0)
    {
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
