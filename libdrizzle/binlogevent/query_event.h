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
	class QueryEvent : public Events 
	{
		public:

//			QueryEvent(TableMapEvent& tableMap) : table_id(0),
			QueryEvent() : proxy_id(0),
					execution_time(0),
					schema_length(0),
					error_code(0)
			{
			}	

			~QueryEvent()
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
			
			uint8_t getType();
			
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
			  * @retval proxy id. 
			  */
		
			uint32_t getProxyId();
			
			/**
			  * @retval execution time.
			  */
			uint32_t getExecutionTime();
			
			/**
			  *@retval schema length.
			  */
			uint8_t getSchemaLength();
			
			/**
			  *@retval error code.
			  */
			uint16_t getErrorCode();
			
			

			//setters
			/** set slave proxy Id
			  *
			  *@param[in] value Proxy Id of slave
			  */

			void setProxyId(uint32_t value);
			
			/** set execution time.
			  *
			  * @param[in] value Execution time of query.
			  */
			void setExecutionTime(uint32_t value);
			
			/** set schema length
			  *
			  * @param[in] value Schema length of query.
			  */
			void setSchemaLength(uint8_t value);
			
			/** set error code.
			  *
			  * @param[in] value Error code if any error occurred.
			  */
			void setErrorCode(uint16_t value);




		private:
			uint32_t proxy_id;  
			uint32_t execution_time;
			uint8_t schema_length; 
			uint16_t error_code;

	}; // query event
} /*namespace binlogevent*/
