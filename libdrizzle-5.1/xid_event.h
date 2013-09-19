/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: parse xid event
 *
 */
#include<cstring>
#include<inttypes.h>

#ifndef event_interface
#define event_interface
#include"event_interface.h"
#endif
using namespace std;

#ifndef XID_EVENT
#define XID_EVENT
//using namespace binlogevent;
namespace binlogevent
{
	class XidEvent : public Events 
	{
		public:

			XidEvent() : Xid(-1)
		{
		}

			~XidEvent()
			{
			}


			/**
			 * Get the raw data and call all the setters with
			 * appropriate value
			 *
			 * @param[in] data Raw data from binglog.
			 */
			DRIZZLE_API
			void initWithData(const unsigned char * data);


			//getters

			/**
			 * @retval timestamp of event.
			 */
			DRIZZLE_API
			uint32_t getTimestamp();
			/**
			 * @retval type of event.
			 */
			DRIZZLE_API
			enum_event_type getType();
			/**
			 * @retval server-id of the originating mysql-server. Used to filter out events in circular replication.
			 */
			DRIZZLE_API
			uint32_t getServerId();
			/**
			 * @retval of position of the next event .
			 */
			DRIZZLE_API
			uint32_t getLogPos();
			/**
			 * @retval Binlog Event Flag from header
			 */
			DRIZZLE_API
			uint16_t getFlagH();
			
			/**
			 * @retval transaction id 
			 */
			DRIZZLE_API
			uint64_t getXid();
			
			/**
			 * set transaction id 
			 */
			DRIZZLE_API
			void setXid(uint64_t value);


		private:
			uint64_t Xid;

	}; // Xidevent 
} /*namespace binlogevent*/
#endif
