#include<cstring>
#include<inttypes.h>

#ifndef event_interface
#define event_interface
#include"event_interface.h"
#endif
using namespace std;


//using namespace binlogevent;
namespace binlogevent
{
	class TableMapEvent : public Events 
	{
		public:

			TableMapEvent() : table_id(0),
			flag(0),
			schema_name_len(0),
			schema_name(NULL),
			table_name_len(0),
			table_name(NULL),
			column_count(0),
			column_type_def(NULL)
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
			 * @retval length of the schema name 
			 */
			int getSchemaNameLen();
			/**
			 * @retval schema name 
			 */
			char * getSchemaName();
			/**
			 * @retval length of table name
			 */
			int getTableNameLen();
			/**
			 * @retval table name 
			 */
			char * getTableName();
			/**
			 * In funtion : conversion of length-encoded integer into its numeric value
			 * @retval number of columns in the table map  
			 */
			uint64_t getColumnCount();
			/**
			 * @retval array of column definitions, one byte per field type  
			 */
			 uint8_t * getColumnTypeDef();

			//setters

			void setTableId(uint64_t value);
			/**
			 * set flag_ph.
			 *
			 * @param[in] value Post header flag.
			 */
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
