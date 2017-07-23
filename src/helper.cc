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
#include "config.h"
#include <iostream>
#include "src/common.h"
#include <errno.h>
#include <string.h>
#include <sstream>
#include <libdrizzle-redux/helper.h>

using namespace std;

char * getString(int pos,int len,const unsigned char * data)
{
	if((int)sizeof(data)-pos<len)
	{
		return NULL;
	}
	char *tmp = (char *)malloc(sizeof(char)*(len));
	int i;
	for(i=pos;i<pos+len;i++)
	{
		tmp[i-pos]=data[i];
	}
	tmp[i-pos]='\0';
	return tmp;
}

std::string getIntToStr(uint64_t num)
{
	std::stringstream ss;
	ss << num;
	std::string str=ss.str();
	return str;
}

uint64_t getEncodedLen(int& pos, const unsigned char *data)
{
	uint64_t len=0;
	if((int)sizeof(data)-pos<1)
	{
		return 0;
	}

	int colBytes = bytes_col_count((uint32_t)data[pos]);
	if((int)sizeof(data)-pos-1<colBytes)
	{
		pos++;
		return 0;
	}
	switch(colBytes)
	{
		case 1:
			len= (uint64_t)data[pos];
			break;

		case 2:
			len= (uint64_t)readBytes<uint16_t>(pos+1,data);
			break;
		case 3:
			len= (uint64_t)readBytes<uint32_t, 3>(pos+1,data);
			break;
		case 8:
			len= readBytes<uint64_t>(pos+1,data);
			break;
		default:
			break;
	}
	pos+=colBytes + ((colBytes>1)? 1 : 0); // include first byte if colCount>1

	return len;
}

bool getNextBit(uint8_t& val)
{
	val = val >> 1;
	return (val & 1);
}
int getBoolArray(bool *arr, const unsigned char *data, int start_pos, int _byte,
	int _bit)
{
	if((int)sizeof(data)-start_pos<_byte)
	{
		return -1;
	}
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

int lookup_metadata_field_size(drizzle_field_type_t field_type)
{
    switch ( field_type )
    {
        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_DATETIME:
        case DRIZZLE_COLUMN_TYPE_DECIMAL: // Pre 5.0
        case DRIZZLE_COLUMN_TYPE_INT24:
        case DRIZZLE_COLUMN_TYPE_LONG:
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
        case DRIZZLE_COLUMN_TYPE_NULL:
        case DRIZZLE_COLUMN_TYPE_SHORT:
        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_TINY:
        case DRIZZLE_COLUMN_TYPE_YEAR:
            return 0;

        // Note that the following column types were added in v5.0
        // - DRIZZLE_COLUMN_TYPE_DATETIME2
        // - DRIZZLE_COLUMN_TYPE_TIME2
        // - DRIZZLE_COLUMN_TYPE_TIMESTAMP2
        //
        // The column types and their  metadata byte size is however not
        // covered in the accessible MySQL source code documentation,
        // (https://is.gd/IPzVrf), cf.
        // "Table_map_event column types: numerical identifier and metadata"
        //
        // Instead the metadata byte size of 1 is based on how MaxScale
        // handles these column types, cf. (avro_rbr.c)
        case DRIZZLE_COLUMN_TYPE_BLOB:
        case DRIZZLE_COLUMN_TYPE_DATETIME2:
        case DRIZZLE_COLUMN_TYPE_DOUBLE:
        case DRIZZLE_COLUMN_TYPE_FLOAT:
        case DRIZZLE_COLUMN_TYPE_GEOMETRY:
        case DRIZZLE_COLUMN_TYPE_TIME2:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
            return 1;

        case DRIZZLE_COLUMN_TYPE_BIT:
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
        case DRIZZLE_COLUMN_TYPE_STRING:
        case DRIZZLE_COLUMN_TYPE_VARCHAR:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
            return 2;

        // The column types below are only used internally at the MySQL. Added
        // so enumeration values of drizzle_column_t are handled in order to
        // satisfy compile flag -Werror=switch-enum
        case DRIZZLE_COLUMN_TYPE_NEWDATE:
		case DRIZZLE_COLUMN_TYPE_ENUM:
		case DRIZZLE_COLUMN_TYPE_SET:
		case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
		case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
		case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
        default:
            return 0;
    }
}

drizzle_field_byte_t lookup_field_bytes(drizzle_field_type_t field_type)
{
    switch ( field_type )
    {
        case DRIZZLE_COLUMN_TYPE_DECIMAL:
        case DRIZZLE_COLUMN_TYPE_VARCHAR:
        case DRIZZLE_COLUMN_TYPE_BIT:
        case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
        case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
        case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
        case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
        case DRIZZLE_COLUMN_TYPE_BLOB:
        case DRIZZLE_COLUMN_TYPE_GEOMETRY:
        case DRIZZLE_COLUMN_TYPE_VAR_STRING:
        case DRIZZLE_COLUMN_TYPE_STRING:
            return LEN_ENC_STR;

        case DRIZZLE_COLUMN_TYPE_TINY:
        case DRIZZLE_COLUMN_TYPE_YEAR:
            return READ_1_BYTE;

        case DRIZZLE_COLUMN_TYPE_SHORT:
            return READ_2_BYTE;

        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_TIME:
        case DRIZZLE_COLUMN_TYPE_TIME2 :
        case DRIZZLE_COLUMN_TYPE_INT24 :
        case DRIZZLE_COLUMN_TYPE_NEWDATE :
            return READ_3_BYTE;

        case DRIZZLE_COLUMN_TYPE_LONG:
        case DRIZZLE_COLUMN_TYPE_FLOAT:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP2 :
        case DRIZZLE_COLUMN_TYPE_DATETIME:
            return READ_4_BYTE;

        case DRIZZLE_COLUMN_TYPE_DATETIME2 :
            return READ_5_BYTE;

        case DRIZZLE_COLUMN_TYPE_DOUBLE:
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
            return READ_8_BYTE;

        case DRIZZLE_COLUMN_TYPE_NULL:
        case DRIZZLE_COLUMN_TYPE_ENUM:
        case DRIZZLE_COLUMN_TYPE_SET:
        default:
            return NOT_FOUND;
    }
}
