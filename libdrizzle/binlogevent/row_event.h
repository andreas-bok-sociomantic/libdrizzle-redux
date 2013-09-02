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
namespace binlogevent
{
	class RowEvent : public Events 
	{
		public:

			RowEvent(uint8_t *col_type) : table_id(0),
					flag(0),
					column_count(0),
					column_bitmap(0)
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
			virtual void initWithData(const unsigned char * data);

			//getters

			/**
			  * @retval timestamp of event.
			  */
			
			uint32_t getTimestamp();
			/**
			  * @retval type of event.
			  */
			
			 enum_event_type getType();
			
			/**
			  * @retval server-id of the originating mysql-server. Used to filter out events in circular replication.
			  */
			uint32_t getServerId();
			
			/**
			  * @retval of size of event.
			  */
			uint32_t getEventSize();
			/**
			  * @retval of position of the next event .
			  */
			uint32_t getLogPos();
			
			/**
			  * @retval Binlog Event Flag from header
			  */
			uint16_t getFlagH();

			/**
			  * @retval numeric table id 
			  */
		
			uint64_t getTableId();
			
			/**
			  * @retval flag of post header 
			  */
			uint16_t getFlagPh();
			
			/**
			  * In funtion : conversion of length-encoded integer into its numeric value
			  * @retval number of columns in the table map  
			 */
			uint64_t getColumnCount();
			
			/**
			  * @retval array of column definitions, one byte per field type  
			  */
			
			int * getColumnPresentBitmap();

			RowVector getRows();

			

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
			void setColumnPresentBitmap(int * value);
			
			/** set null_bitmap.
			  *
			  * @param[in] null bitmap Array 
			  */
			void setNullBitmap(bool * value);

			void setRowsCount(int value);




		private:
			uint8_t * column_type;
			uint64_t table_id;  
			uint16_t flag; //post header
			uint64_t column_count; 
			int * column_bitmap;
			bool * column_null_bitmap;
			bool * null_bitmap;
			RowIterator rows;

	}; // RowEvent
} /*namespace binlogevent*/
