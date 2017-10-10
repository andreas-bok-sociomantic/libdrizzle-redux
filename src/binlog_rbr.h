/*
 * Drizzle Client & Protocol Library
 *
 * Copyright: Copyright (c) 2017 sociomantic labs GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 *     * The names of its contributors may not be used to endorse or
 * promote products derived from this software without specific prior
 * written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <unordered_map>
#include <vector>
#include <iterator>
#include <cstdarg>



struct tablename_rows_events_map
{
    typedef std::vector<drizzle_binlog_rows_event_st** > vec_ptr_row_events;
    typedef std::unordered_map<const char*, vec_ptr_row_events>
        map_tablename_vec_row_events_ptr;

    /**
     * Flag which indicates whether the client has changed the table from which
     * to retrieve row events
     */
    bool table_changed;

    /**
     * The id of the db table from which row events are retrieved
     */
    uint64_t table_id;

    /**
     * The name of the db table from which row events are retrieved
     */
    char table_name[DRIZZLE_MAX_TABLE_SIZE];

    /**
     * Mapping between table name and associated row events
     */
    map_tablename_vec_row_events_ptr mapping;
    map_tablename_vec_row_events_ptr::iterator mapping_it;

    /**
     * Iterator to access the vector of row events
     */
    vec_ptr_row_events::iterator row_events_it;

    /**
     * A pointer to the current vector of row events being requested
     */
    vec_ptr_row_events *curr_row_events;


    /**
     * @brief      Constructor
     */
    tablename_rows_events_map() :
        table_changed(false),
        table_id(0)

    {
        table_name[0] = '\0';
    }


    /**
     * @brief      Determines if a mapping for a table exists
     *
     * @param[in]  tablename  A tablename
     *
     * @return     True if has a mapping exists, False otherwise.
     */
    bool has_table(const char *tablename)
    {
        return mapping.find(tablename) != mapping.end();
    }

    /**
     * @brief      Sets the rows events iterator.
     *
     * @param[in]  tablename  A tablename
     *
     * @return     True if the table was found, False otherwise
     */
    bool set_rows_events_it(const char *tablename, drizzle_list_position_t
        pos=DRIZZLE_LIST_BEGIN)
    {
        if (!has_table(tablename))
        {
            return false;
        }
        else
        {
            curr_row_events = &mapping.find(tablename)->second;
            row_events_it = pos == DRIZZLE_LIST_BEGIN ?
                curr_row_events->begin() :
                curr_row_events->end();
            if (table_name != tablename)
            {
                strcpy(table_name, tablename);
                table_changed = true;
            }

            return true;
        }
    }

    /**
     * @brief      Get the next row event from a table
     *
     * @param[in]  tablename  The tablename
     *
     * @return     A pointer to a event struct, False if no more
     *             events are available
     */
    drizzle_binlog_rows_event_st* next_row_event(const char *tablename)
    {
        if (!set_rows_events_it(tablename))
        {
            return NULL;
        }

        if (table_changed)
        {
            return **(row_events_it++);
        }
        else if (next(row_events_it) != curr_row_events->end())
        {
            return **(++row_events_it);
        }
        else
        {
            return NULL;
        }
    }

    /**
     * @brief      Adds a mapping between a table and an associated row event
     *             struct
     *
     * @param      rows_event  Pointer to a rows event struct
     */
    void add_mapping(drizzle_binlog_rows_event_st *rows_event)
    {
        if (mapping.find(rows_event->table_name) == mapping.end())
        {
            vec_ptr_row_events vec;
            mapping.insert(std::make_pair(rows_event->table_name, vec));
        }

        auto vec_rows = &mapping.find(rows_event->table_name)->second;
        vec_rows->push_back(&rows_event);
        printf("add_mapping: %ld\n", vec_rows->size());
    }


    size_t row_events_count(const char* _table_name)
    {

        return has_table(_table_name) ?
            mapping.find(_table_name)->second.size() : 0;
    }

    /**
     * @brief      Reset the state
     *
     * @detailed   Clears the mapping from table to parsed row events
     */
    void reset()
    {
        table_changed = false;
        table_name[0] = '\0';

        mapping_it = mapping.begin();
        for(;mapping_it != mapping.end(); mapping_it++)
        {
            mapping_it->second.clear();
        }

        mapping.clear();
    }
};

struct drizzle_binlog_rbr_st
{
    typedef std::unordered_map<uint64_t, drizzle_binlog_tablemap_event_st*>
        map_tablemap_events;
    typedef std::unordered_map<const char*, drizzle_binlog_tablemap_event_st*>
        map_tablename_tablemap_event;
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

    /** pointer to binlog struct */
    drizzle_binlog_st *binlog;

    //** callback function */
    drizzle_binlog_rbr_fn *binlog_rbr_fn;

    //** xid event struct */
    drizzle_binlog_xid_event_st xid_event;

    //** query event struct  */
    drizzle_binlog_query_event_st query_event;

    //** mapping from table id to table map event struct */
    map_tablemap_events tablemap_events;

    //** mapping from table name to table map event struct */
    map_tablename_tablemap_event tablename_tablemap_event;

    //** mapping between a table name to the table's rows event structs     */
    tablename_rows_events_map tablename_rows_events;

    //** vector of parsed row event structs */
    vec_row_events rows_events;

    //** iterator for accessing the vector of row events structs */
    rows_events_iterator rows_event_it;

    //* total number of row event in the current binlog event group.
    //  If row events are not buffered this is always 1  */
    size_t row_events_count_;

    //** number of parsed row events. */
    size_t rows_events_parsed;

    //** the id of the table from which to get row events */
    uint64_t current_tablemap_id;

    //** buffer used for formatting */
    char fmt_buffer[1024];

    //** default database as specified with drizzle_create */
    char db[DRIZZLE_MAX_DB_SIZE];

    /**
     * @brief      Constructor
     */
    drizzle_binlog_rbr_st() :
        binlog(NULL),
        binlog_rbr_fn(NULL),
        row_events_count_(0),
        current_tablemap_id(0)
    {
    }

    /**
     * @brief      Destroys the object.
     */
    ~drizzle_binlog_rbr_st()
    {

    }

    /**
     * @brief      Reset the state
     *             If free_rows is true all rows events are freed
     *
     * @param[in]  free_rows  bool flag
     */
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
     * @brief      Gets a tablemap event by name
     *
     *             A schema name can be specified to avoid conflicts with
     *             duplicate tables.
     *
     * @param[in]  table_name  The table name
     * @param[in]  <unnamed>   schema name, optional
     *
     * @return     The tablemap event.
     */
    drizzle_binlog_tablemap_event_st *get_tablemap_event(const char* table_name);

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
     * @brief      Number of row events count for a specific table
     *
     * @param[in]  table_name  name of a db table
     *
     * @return     The row events count.
     */
    size_t get_row_events_count(const char* table_name);

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

    void add_tablemap_event(drizzle_binlog_tablemap_event_st *event);

    /**
     * Adds a drizzle binlog event
     *
     * @param      binlog_event  a drizzle binlog event struct
     */
    void add_binlog_event(drizzle_binlog_event_st* binlog_event);



    /**
     * @brief      Return a schema and table name formatted as
     *             <schema.table>
     *
     * @param[in]  table_name  The table name
     *
     * @return     string consisting of schema.table
     */
    const char *schema_table_name(const char *table_name_)
    {
        assert(table_name_ != NULL);
        sprintf(&fmt_buffer[0], "%s.%s", db, table_name_);
        return &fmt_buffer[0];
    }
};

/*size_t drizzle_binlog_rbr_row_events_count_(drizzle_binlog_rbr_st *binlog_rbr,
                                           ...);*/

/*#define __drizzle_binlog_rbr_row_events_count__(...) \
    drizzle_binlog_rbr_row_events_count_(binlog_rbr, ##__VA_ARGS__, NULL)
*/
