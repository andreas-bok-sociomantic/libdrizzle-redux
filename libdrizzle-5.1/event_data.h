/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: Returns objects of every event
 *
 */
#include<iostream>
#include<cstring>
#include<inttypes.h>

#include"table_map_event.h"
#include"row_event.h"
#include"query_event.h"
#include"xid_event.h"


using namespace std;
//using namespace binlogevent;

namespace binlogevent
{
	class EventData 
	{
		public:

			EventData(const unsigned char* data)
			{
				_data=data;
			}

			~EventData()
			{
			}
			/** Method which sets the field values
			  * for table map event. calls initWithData
			  * of TableMapEvent
			  *
			  * @param[in] tableMap A reference to TableMapEvent object.
			  */
			DRIZZLE_API
			void getTableMap(TableMapEvent&  tableMap);
			DRIZZLE_API
			void getWriteEvent(RowEvent&  writeEvent);
			DRIZZLE_API
			void getUpdateEvent(RowEvent&  updateEvent);
			DRIZZLE_API
			void getDeleteEvent(RowEvent&  deleteEvent);
			DRIZZLE_API
			void getQueryEvent(QueryEvent&  queryEvent);
			DRIZZLE_API
			void getXidEvent(XidEvent&  xidEvent);
			
			DRIZZLE_API
			void setData(const unsigned char* _data);


		private:
			/* Raw Event Data  */
			const unsigned char* _data;


	};
} /*namespace binlogevent*/

