/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: parsing of header (common for each event)
 *
 */

#include<iostream>
#include <libdrizzle-5.1/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include<string.h>

#ifndef event_header
#define event_header
#include"event_header.h"
#endif

using namespace std;
using namespace binlogevent;


int EventHeader::setHeader(const unsigned char* data)
{
	// Total bytes to read: 19
	int start_pos = 0;  
        timestamp= getByte4(start_pos,data);
	if(timestamp==UINT_MAX)
		return -1;
	start_pos+=4; // 4 byte for timestamp.
	
	if(sizeof(data)-start_pos<0)
		return -1;
	int tmp=(uint8_t)data[start_pos];
	type= (enum_event_type)tmp;
	start_pos+=1; // 1 byte for type of evnet.
	
	server_id= getByte4(start_pos,data);
	if(server_id==UINT_MAX)
		return -1;
	start_pos+=4; // 4 byte of server ID.
	
	event_size= getByte4(start_pos,data);
	if(event_size==UINT_MAX)
		return -1;
	start_pos+=4; // 4 byte for event size.
	
	log_pos= getByte4(start_pos,data);
	if(log_pos==UINT_MAX)
		return -1;
	start_pos+=4;// 4 byte for getting possion of next event.
	
	flag= getByte2(start_pos,data);
	if(flag==USHRT_MAX)
		return -1;
	start_pos+=2; // 2 bytes for getting flag

	return start_pos;	
}
