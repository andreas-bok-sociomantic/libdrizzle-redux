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

#include "config.h"
#include <iostream>
#include "src/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <libdrizzle-redux/event_header.h>

using namespace std;
using namespace binlogevent;


int EventHeader::setHeader(const unsigned char* data)
{
	// Total bytes to read: 19
	int start_pos = 0;
        timestamp= readBytes<uint32_t>(start_pos,data);
	if(timestamp==UINT_MAX)
		return -1;
	start_pos+=4; // 4 byte for timestamp.

	if((int)(sizeof(data) - start_pos) < 0)
		return -1;
	int tmp=(uint8_t)data[start_pos];
	type= (drizzle_binlog_event_types_t)tmp;
	start_pos+=1; // 1 byte for type of evnet.

	server_id= readBytes<uint32_t>(start_pos,data);
	if(server_id==UINT_MAX)
		return -1;
	start_pos+=4; // 4 byte of server ID.

	event_size= readBytes<uint32_t>(start_pos,data);
	if(event_size==UINT_MAX)
		return -1;
	start_pos+=4; // 4 byte for event size.

	log_pos= readBytes<uint32_t>(start_pos,data);
	if(log_pos==UINT_MAX)
		return -1;
	start_pos+=4;// 4 byte for getting possion of next event.

	flag= readBytes<uint16_t>(start_pos,data);
	if(flag==USHRT_MAX)
		return -1;
	start_pos+=2; // 2 bytes for getting flag

	return start_pos;
}
