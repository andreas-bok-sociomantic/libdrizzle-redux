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
#include <libdrizzle-5.1/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include<string.h>
#include"event_data.h"

using namespace std;
using namespace binlogevent;

void EventData::getTableMap(TableMapEvent& tableMap)
{
	tableMap.initWithData(_data);
}

void EventData::getRowEvent(RowEvent& writeEvent)
{
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
