/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: creation of Event object (global) 
 *
 */
#include<iostream>
#include<cstring>
#include<inttypes.h>

#ifndef event_header
#define event_header
#include"event_header.h"
#endif

using namespace std;
//using namespace binlogevent;

namespace binlogevent
{
	class Events
	{

		public:

			Events()
			{
			}
			virtual ~Events()
			{
			}

			/** Every event class should implement
			 * this function.
			 *
			 * @param[in] data Raw event data.
			 */
			virtual void initWithData(const unsigned char *data)=0;

			EventHeader header;

	};
}  

