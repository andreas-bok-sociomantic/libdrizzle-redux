/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: parsing of row events like write event, update event, delete event
 *
 */
#include<cstring>
#include<inttypes.h>

#ifndef event_interface
#define event_interface
#include"event_interface.h"
#endif

#include"row_iterator.h"

using namespace std;


//using namespace binlogevent;

#ifndef ROW_EVENT
#define ROW_EVENT

namespace binlogevent
{
	class RowEvent : public Events 
	{
		public:

			RowEvent(uint8_t *col_type) : 
					table_id(-1),
					flag(-1),
					column_count(-1),
					column_null_bitmap(NULL),
					column_bitmap(NULL)
			{
				column_type=col_type;
			}	

			~RowEvent()
			{
			}


			/** Get the raw data and call all the setters with
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
			  * @retval of size of event.
			  */
			DRIZZLE_API
			uint32_t getEventSize();
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
			  * @retval numeric table id 
			  */
		
			DRIZZLE_API
			uint64_t getTableId();
			
			/**
			  * @retval flag of post header 
			  */
			DRIZZLE_API
			uint16_t getFlagPh();
			
			/**
			  * In funtion : conversion of length-encoded integer into its numeric value
			  * @retval number of columns in the table map  
			 */
			DRIZZLE_API
			uint64_t getColumnCount();
			
			/**
			  * @retval bitmap of column present
			  */
			
			DRIZZLE_API
			bool * getColumnPresentBitmap();

			DRIZZLE_API
			RowVector getRows();
			
			DRIZZLE_API
			int getRowEventType();

			//RowVector getAllRows();
			//Row getRow();

			

			//setters

			void setTableId(uint64_t value);
			
			/** set flag_ph.
			  *
			  * @param[in] value Post header flag.
			  */
			void setFlagPh(uint16_t value);
			
			/** set column_count.
			  *
			  * @param[in] value Column count in table map.
			  */
			void setColumnCount(uint64_t value);
			
			/** set column_bitmap.
			  *
			  * @param[in] bitmap Array 
			  */
			void setColumnPresentBitmap(bool * value);
			
			/** set null_bitmap.
			  *
			  * @param[in] null bitmap Array 
			  */
			void setNullBitmap(bool * value);

			void setRowEventType(int value);




		private:
			uint8_t * column_type;
			uint64_t table_id;  
			uint16_t flag; //post header
			uint64_t column_count; 
			bool * column_null_bitmap;
			bool * column_bitmap;
			bool * null_bitmap;
			RowIterator rows;
			int row_event_type; // write-> 1, delete-> 2, update->3

	}; // RowEvent
} /*namespace binlogevent*/
#endif
