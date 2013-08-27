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

void EventData::getWriteEvent(WriteEvent& writeEvent)
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
