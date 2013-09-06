/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: parse xid event
 *
 */
#include<iostream>
#include <libdrizzle-5.1/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include<string.h>

#include"xid_event.h"


#ifndef HELPER
#define HELPER

#include"helper.h"

#endif

using namespace std;
using namespace binlogevent;



void XidEvent::initWithData(const unsigned char* data)
{

	int start_pos = header.setHeader(data);

	setXid(getByte8(start_pos,data));
}

// getters

uint32_t XidEvent::getTimestamp()
{
	return  header.timestamp;
}
enum_event_type XidEvent::getType()
{
	return (enum_event_type)header.type; 
}
uint32_t XidEvent::getServerId()
{
	return header.server_id;
}
uint32_t XidEvent::getLogPos()
{
	return header.log_pos;
}
uint16_t XidEvent::getFlagH()
{
	return header.flag; 
}
uint64_t XidEvent::getXid()
{
	return Xid;
}

//setters

void XidEvent::setXid(uint64_t value)
{
	Xid = value;
}
