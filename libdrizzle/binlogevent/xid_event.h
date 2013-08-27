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
	class XidEvent : public Events 
	{
		public:

			XidEvent() : Xid(0)
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
			 * @retval transaction id 
			 */
			uint64_t getXid();
			
			/**
			 * set transaction id 
			 */
			void setXid(uint64_t value);


		private:
			uint64_t Xid;

	}; // Xidevent 
} /*namespace binlogevent*/
