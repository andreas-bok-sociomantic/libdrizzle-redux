/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: parse table map event
 *
 */
#include<cstring>
#include<inttypes.h>

#ifndef event_interface
#define event_interface
#include"event_interface.h"
#endif
using namespace std;
#ifndef HELPER
#define HELPER

#include"helper.h"

#endif

#ifndef TABLE_MAP
#define TABLE_MAP
//using namespace binlogevent;
namespace binlogevent
{
	class TableMapEvent : public Events 
	{
		public:

			TableMapEvent() : table_id(-1),
			flag(-1),
			schema_name_len(-1),
			schema_name(NULL),
			table_name_len(-1),
			table_name(NULL),
			column_count(-1),
			column_type_def(NULL),
			column_meta_data(NULL)
		{
		}

			~TableMapEvent()
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
			 * @retval length of the schema name 
			 */
			DRIZZLE_API
			int getSchemaNameLen();
			/**
			 * @retval schema name 
			 */
			DRIZZLE_API
			char * getSchemaName();
			/**
			 * @retval length of table name
			 */
			DRIZZLE_API
			int getTableNameLen();
			/**
			 * @retval table name 
			 */
			DRIZZLE_API
			char * getTableName();
			/**
			 * In funtion : conversion of length-encoded integer into its numeric value
			 * @retval number of columns in the table map  
			 */
			DRIZZLE_API
			uint64_t getColumnCount();
			/**
			 * @retval array of column definitions, one byte per field type  
			 */
			DRIZZLE_API
			 uint8_t * getColumnTypeDef();
			
			 /**
			 * @retval enum {INT,STRING} as a type of column  
			 */

			DRIZZLE_API
			 enum_col_type getColType(int colNo);

			//setters

			DRIZZLE_API
			void setTableId(uint64_t value);
			/**
			 * set flag_ph.
			 *
			 * @param[in] value Post header flag.
			 */
			DRIZZLE_API
			void setFlagPh(uint16_t value);
			/**
			 * set schema_name_len.
			 *
			 * @param[in] value Length of the schema name. 
			 */
			void setSchemaNameLen(uint8_t value);
			/**
			 * set schema_name.
			 *
			 * @param[in] value Schema Name or Database name.
			 */
			void setSchemaName(char *value);
			/**
			 * set table_name_len.
			 *
			 * @param[in] value Table name length.
			 */
			void setTableNameLen(uint8_t value);
			/**
			 * set table_name.
			 *
			 * @param[in] value Table name.
			 */
			void setTableName(char *value);
			/**
			 * set column_count.
			 *
			 * @param[in] value Column count in table map.
			 */
			void setColumnCount(uint64_t value);
			/**
			 * set column_type_def.
			 *
			 * @param[in] value Array of column definitions, one byte per field type
			 */
			void setColumnTypeDef(uint8_t * value);



		private:
			uint64_t table_id;  
			uint16_t flag; //post header
			uint8_t schema_name_len;
			char * schema_name;
			uint8_t table_name_len;
			char * table_name;
			uint64_t column_count; 
			uint8_t * column_type_def;
			uint64_t * column_meta_data;

	}; // tablemapevent
} /*namespace binlogevent*/
#endif
