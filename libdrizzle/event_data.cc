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
#include "config.h"
#include<iostream>
#include "libdrizzle/common.h"
#include "libdrizzle/commonapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include<string.h>
//#include<libdrizzle-5.1/event_data.h>
#define EVENT_DATA
using namespace std;
using namespace binlogevent;

void EventData::setData(const unsigned char* data)
{
	_data=data;
}

void EventData::getTableMap(TableMapEvent& tableMap)
{
	tableMap.initWithData(_data);
}

void EventData::getUpdateEvent(RowEvent& updateEvent)
{
	        updateEvent.setRowEventType(3); // type 3
		        updateEvent.initWithData(_data);
}

void EventData::getDeleteEvent(RowEvent& deleteEvent)
{
	        deleteEvent.setRowEventType(2); // type 2
		        deleteEvent.initWithData(_data);
}

void EventData::getWriteEvent(RowEvent&  writeEvent)
{
	        writeEvent.setRowEventType(1); // type 1
		        writeEvent.initWithData(_data);
}

void EventData::getQueryEvent(QueryEvent& queryEvent)
{
	queryEvent.initWithData(_data);
}
void EventData::getXidEvent(XidEvent& xidEvent)
{
	xidEvent.initWithData(_data);
}
