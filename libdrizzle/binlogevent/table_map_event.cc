/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: parse table map event
 *
 */
#include<iostream>
#include <libdrizzle-5.1/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include<string.h>

#ifndef table_map

#define table_map

#include"table_map_event.h"

#endif


using namespace std;
using namespace binlogevent;



void TableMapEvent::initWithData(const unsigned char* data)
{
	uint64_t tmp;
	char *tmp_char;
	int tmp_int;
	int start_pos = header.setHeader(data);
	if(start_pos==-1)
		return;

	tmp = getByte6(start_pos,data);	
	if(tmp==UINT_MAX)
		return;
	setTableId((uint64_t)tmp);
	start_pos+=6;// 6 byte for table id.
	
	tmp = getByte2(start_pos,data);	
	if(tmp==USHRT_MAX)
		return;
	setFlagPh((uint16_t)tmp);
	start_pos+=2;// 2 byte for post-header flag.

	if(sizeof(data)-start_pos<1)
		return;
	setSchemaNameLen((uint8_t)data[start_pos]);
	start_pos+=1;// 1 byte for schema name length.
	
	tmp_char = getString(start_pos,schema_name_len,data);
	if(tmp_char==NULL)
		return;
	setSchemaName(tmp_char);
	start_pos+= schema_name_len; //schema_name_len byte for schema name.

	//  data[start_pos+getSchemaNameLen()] is Null
	start_pos+=1; //  +1 for Null.
	
	if(sizeof(data)-start_pos<1)
		return;
	setTableNameLen((uint8_t)data[start_pos]);
	start_pos+=1;// 1 byte for table name length.
	
	tmp_char = getString(start_pos,schema_name_len,data);
	if(tmp_char==NULL)
		return;
	setTableName(tmp_char);
	start_pos+=table_name_len+1; // +1 for null

	tmp_int = getEncodedLen(start_pos,data);
	if(tmp_int==0)
		return;
	setColumnCount(getEncodedLen(start_pos,data)); // start_pos will also get updated


	if(sizeof(data)-start_pos<getColumnCount())
		return;
	uint8_t *tmp_array = (uint8_t *)(malloc(sizeof(uint8_t)*getColumnCount()));
	for(int i=0;i<getColumnCount();i++)
	{
		tmp_array[i]=(uint8_t)data[start_pos+i];
	}
	setColumnTypeDef(tmp_array);
	start_pos+=column_count;


	int metaSize= getEncodedLen(start_pos,data);
	if(metaSize==0)
		return;
	uint64_t *column_meta_data = (uint64_t *)(malloc(sizeof(uint64_t)*column_count));
	
	if(sizeof(data)-start_pos<column_count)
		return;
	for(int col=0;col<column_count;col++)
	{
		int type= column_type_def[col];
		int nextBytes= lookup_metadata_field_size((enum_field_types)type);
		int metaData=0;
		switch(nextBytes)
		{
			case 1:
				metaData= (int)data[start_pos];
				break;
			case 2:	
				metaData= (int)getByte2(start_pos,data);
				break;
			default:
				break;
		}
		column_meta_data[col]= metaData;
		start_pos+=nextBytes;
	}

}

// getters

uint32_t TableMapEvent::getTimestamp()
{
	return  header.timestamp;
}
enum_event_type TableMapEvent::getType()
{
	return (enum_event_type)header.type; 
}
uint32_t TableMapEvent::getServerId()
{
	return header.server_id;
}
uint32_t TableMapEvent::getLogPos()
{
	return header.log_pos;
}
uint16_t TableMapEvent::getFlagH()
{
	return header.flag; 
}
uint64_t TableMapEvent::getTableId()
{
	return table_id;
}
uint16_t TableMapEvent::getFlagPh()
{
	return flag;
}
int TableMapEvent::getSchemaNameLen()
{
	return (int)schema_name_len;
}
char * TableMapEvent::getSchemaName()
{
	return schema_name;
}
int TableMapEvent::getTableNameLen()
{
	return (int)table_name_len;
}
char * TableMapEvent::getTableName()
{
	return table_name;
}
uint64_t TableMapEvent::getColumnCount()
{
	return column_count;
}
uint8_t * TableMapEvent::getColumnTypeDef()
{
	return column_type_def;
}

enum_col_type TableMapEvent::getColType(int colNo)
{
	enum_field_bytes num;
	num = lookup_field_bytes((enum_field_types)column_type_def[colNo]);
	switch(num)
	{
		 case LEN_ENC_STR:
			 return (enum_col_type)1;
		 default:
			 return (enum_col_type)2;
	}
}


//setters

void TableMapEvent::setTableId(uint64_t value)
{
	table_id = value;
}
void TableMapEvent::setFlagPh(uint16_t value)
{
	flag = value;
}
void TableMapEvent::setSchemaNameLen(uint8_t value)
{
	schema_name_len = value;
}
void TableMapEvent::setSchemaName(char *value)
{
	schema_name = value;
}
void TableMapEvent::setTableNameLen(uint8_t value)
{
	table_name_len = value;
}
void TableMapEvent::setTableName(char *value)
{
	table_name = value;
}
void TableMapEvent::setColumnCount(uint64_t value)
{
	column_count = value;
}
void TableMapEvent::setColumnTypeDef(uint8_t * value)
{
	column_type_def = value;
}
