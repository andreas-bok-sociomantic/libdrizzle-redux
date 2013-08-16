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
	int start_pos = 0;  
        timestamp= getByte4(start_pos,data);
	start_pos+=4; // 4 byte for timestamp.
	
	int tmp=(uint8_t)data[start_pos];
	type= (enum_event_type)tmp;
	start_pos+=1; // 1 byte for type of evnet.
	
	server_id= getByte4(start_pos,data);
	start_pos+=4; // 4 byte of server ID.
	
	event_size= getByte4(start_pos,data);
	start_pos+=4; // 4 byte for event size.
	
	log_pos= getByte4(start_pos,data);
	start_pos+=4;// 4 byte for getting possion of next event.
	
	flag= getByte2(start_pos,data);
	start_pos+=2; // 2 bytes for getting flag

	return start_pos;	
}
