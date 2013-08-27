#include<iostream>
#include<cstring>
#include<inttypes.h>

#include"table_map_event.h"
#include"write_row_event.h"
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

			void getTableMap(TableMapEvent&  tableMap);
			void getWriteEvent(WriteEvent&  writeEvent);
			void getQueryEvent(QueryEvent&  queryEvent);
			void getXidEvent(XidEvent&  xidEvent);


		private:
			/* Raw Event Data  */
			const unsigned char* _data;


	};
} /*namespace binlogevent*/

