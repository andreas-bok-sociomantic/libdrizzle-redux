/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: parsing of header (common for each event)
 *
 */
#include<iostream>
#include<cstring>
#include<inttypes.h>

#ifndef table_map
#define table_map

//#include"table_map_event.h"

#endif

using namespace std;

namespace binlogevent
{
	class EventHeader 
	{
		public:

			EventHeader()
			{
			}

			~EventHeader()
			{
			}

			int setHeader(unsigned const char* data);

			friend class TableMapEvent;
			friend class WriteEvent;
			friend class QueryEvent;
			friend class XidEvent;

		private:
			uint32_t timestamp; // timestamp of event
			uint8_t type;       // type of event
			uint32_t server_id; // server-id of the originating mysql-server. Used to filter out events in circular replication
			uint32_t event_size;//size of the event (header, post-header, body) 
			uint32_t log_pos;   // position of next event
			uint16_t flag;    //header


	};
} /*namespace binlogevent*/

