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

#ifndef HELPER
#define HELPER

#include"helper.h"

#endif


using namespace std;

#ifndef EVENT_HEADER
#define EVENT_HEADER

namespace binlogevent
{
	class EventHeader 
	{
		public:

			EventHeader():
				timestamp(-1),
				type(-1),
				server_id(-1),
				event_size(-1),
				log_pos(-1),
				flag(-1)
			{
			}

			~EventHeader()
			{
			}

			int setHeader(unsigned const char* data);

			friend class TableMapEvent;
			friend class RowEvent;
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
#endif
