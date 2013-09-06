/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: Utility functions to parse event data
 *
 */
#include<iostream>
#include <libdrizzle-5.1/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include<string.h>
#include"helper.h"

using namespace std;

uint32_t getByte4(int pos,const unsigned char* data)
{
	uint32_t tmpMask = mask(32); // all 32 bits set to 1		

	tmpMask=((uint32_t)data[pos]&tmpMask);
	tmpMask=((uint32_t)data[pos+1]<<8|tmpMask);
	tmpMask=((uint32_t)data[pos+2]<<16|tmpMask);
	tmpMask=((uint32_t)data[pos+3]<<24|tmpMask);
	
	return tmpMask;
}

uint32_t getByte3(int pos,const unsigned char* data)
{
	uint32_t tmpMask = mask(32); // all 32 bits set to 1		

	tmpMask=((uint32_t)data[pos]&tmpMask);
	tmpMask=((uint32_t)data[pos+1]<<8|tmpMask);
	tmpMask=((uint32_t)data[pos+2]<<16|tmpMask);
	
	return tmpMask;
}

uint16_t getByte2(int pos,const unsigned char* data)
{
	uint16_t tmpMask = mask(16); // all 16 bits set to 1		

	tmpMask=((uint16_t)data[pos]&tmpMask);
	tmpMask=((uint16_t)data[pos+1]<<8|tmpMask);

	return tmpMask;
}


uint64_t getByte6(int pos,const unsigned char* data)
{
	uint64_t tmpMask = mask(64); // all 64 bits set to 1		

	tmpMask=((uint64_t)data[pos]&tmpMask);
	tmpMask=((uint64_t)data[pos+1]<<8|tmpMask);
	tmpMask=((uint64_t)data[pos+2]<<16|tmpMask);
	tmpMask=((uint64_t)data[pos+3]<<24|tmpMask);
	tmpMask=((uint64_t)data[pos+4]<<32|tmpMask);
	tmpMask=((uint64_t)data[pos+5]<<40|tmpMask);

	return tmpMask;
}

uint64_t getByte8(int pos,const unsigned char* data)
{
	uint64_t tmpMask = mask(64); // all 64 bits set to 1		

	tmpMask=((uint64_t)data[pos]&tmpMask);
	tmpMask=((uint64_t)data[pos+1]<<8|tmpMask);
	tmpMask=((uint64_t)data[pos+2]<<16|tmpMask);
	tmpMask=((uint64_t)data[pos+3]<<24|tmpMask);
	tmpMask=((uint64_t)data[pos+4]<<32|tmpMask);
	tmpMask=((uint64_t)data[pos+5]<<40|tmpMask);
	tmpMask=((uint64_t)data[pos+5]<<48|tmpMask);
	tmpMask=((uint64_t)data[pos+5]<<56|tmpMask);

	return tmpMask;
}

char * getString(int pos,int len,const unsigned char * data)
{
	char *tmp = (char *)malloc(sizeof(char)*(len));
	int i;
	for(i=pos;i<pos+len;i++)
	{
		tmp[i-pos]=data[i];
	}
	tmp[i-pos]='\0';
	return tmp;
}

uint64_t getEncodedLen(int& pos, const unsigned char *data)
{
	uint64_t len=0;
	int colBytes = bytes_col_count((uint32_t)data[pos]);
	switch(colBytes)
	{   
		case 1:
			len= (uint64_t)data[pos];
			break;

		case 2:
			len= (uint64_t)getByte2(pos+1,data);
			break;
		case 3:
			len= (uint64_t)getByte3(pos+1,data);
			break;
		case 8:
			len= (uint64_t)getByte8(pos+1,data);
			break;
		default:
			break;
	}   
	pos+=colBytes+(colBytes>1)?1:0; // include first byte if colCount>1

	return len;
}

bool getNextBit(uint8_t& val)
{
	val = val >> 1;
	return (val & 1);
}
int getBoolArray(bool arr[],const unsigned char data[],int start_pos,int _byte,int _bit)
{
	int count=0;
	for(int i=0;i<_byte;i++)
	{   
		if(8*i>=_bit)
			break;
		uint8_t number= (uint8_t)data[start_pos+i];
		arr[8*i] = (int)number&(int)1;
		if(arr[8*i]==0)
			count++;
		for(int val=1;val<8;val++)
		{   
			if((8*i+val) >= _bit)
				break;
			arr[8*i+val] = getNextBit(number);
			if(arr[8*i+val]==0)
				count++;
		}   
	}
	return count; // count where bit in not set. (0)

}
int lookup_metadata_field_size(enum_field_types field_type)
{
	switch (field_type)
	{
		case MYSQL_TYPE_DOUBLE:
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_GEOMETRY:
			return 1;
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
			return 2;
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_SET:
		case MYSQL_TYPE_ENUM:
		case MYSQL_TYPE_YEAR:
		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_NULL:
		case MYSQL_TYPE_NEWDATE:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		default:
			return 0;
	}

}

enum_field_bytes lookup_field_bytes(enum_field_types field_type)
{
	switch (field_type)
	{
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_NEWDATE:
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_STRING:
			return LEN_ENC_STR;
		
		case MYSQL_TYPE_TINY:
			return READ_1_BYTE;
		
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_YEAR:
			return READ_2_BYTE;
		
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_INT24:
			return READ_4_BYTE;
		
		case MYSQL_TYPE_DOUBLE:
		case MYSQL_TYPE_LONGLONG:
			return READ_8_BYTE;
	//	case MYSQL_TYPE_NULL: ???


		case MYSQL_TYPE_ENUM:
		case MYSQL_TYPE_SET:
		case MYSQL_TYPE_GEOMETRY:
			return NOT_FOUND;

	}
}
