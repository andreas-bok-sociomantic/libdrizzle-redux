/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: parsing of row events like write event, update event, delete event
 *
 */

#include<iostream>
#include <libdrizzle-5.1/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include<string.h>

#ifndef row_event
#define row_event

#include"row_event.h"

#endif

#ifndef HELPER
#define HELPER

//#include"helper.h"

#endif

using namespace std;
using namespace binlogevent;



void RowEvent::initWithData(const unsigned char* data)
{

	int start_pos = header.setHeader(data);

	setTableId(getByte6(start_pos,data)); 
	start_pos+=6;// 6 byte for table id.

	setFlagPh(getByte2(start_pos,data));
	start_pos+=2;// 2 byte for post-header flag.

	setColumnCount(getEncodedLen(start_pos,data)); // start_pos will also get updated

	int size= (column_count+7)/8; // length of present column bitmap1

	int *tmp = (int *)(malloc(sizeof(int)*size));
	for(int i=0;i<size;i++)
	{
		tmp[i]=(int)data[start_pos+i];
	}
	setColumnPresentBitmap(tmp);

	start_pos+=size;	//null-bitmap
	size= (column_count+7)/8; // length of null bitmap in bytes 

	bool *tmp_bool = (bool *)malloc(sizeof(bool)*column_count);
	for(int i=0;i<size;i++)
	{
		if(8*i>=column_count)
			break;
		uint8_t number= (uint8_t)data[start_pos+i];
		tmp_bool[8*i] = (int)number&(int)1;
		for(int val=1;val<8;val++)
		{
			if((8*i+val) >= column_count)
				break;
			tmp_bool[8*i+val] = getNextBit(number);
		}
	}
	setNullBitmap(tmp_bool);

	start_pos+=size;

	//LOOP for ROWs
	while(1)
	{
		rows.vec_col_val.clear();
		for(int val=0;val<column_count;val++)
		{
			if(null_bitmap[val]==1)
				continue;

			enum_field_bytes num;
			num = lookup_field_bytes((enum_field_types)column_type[val]);
			switch(num)
			{
				case LEN_ENC_STR:
					{
						int len;
						len=getEncodedLen(start_pos,data);
						ColumnValue value(start_pos,start_pos+len-1);
						rows.vec_col_val.push_back(value);
						start_pos+=len; //length of string
						break;
					}
				case READ_1_BYTE:
					{
						ColumnValue value(start_pos,(start_pos+(int)READ_1_BYTE-1));
						rows.vec_col_val.push_back(value);
						start_pos+=(int)READ_1_BYTE;
						break;
					}
				case READ_2_BYTE:
					{
						ColumnValue value(start_pos,(start_pos+(int)READ_2_BYTE-1));
						rows.vec_col_val.push_back(value);
						start_pos+=(int)READ_2_BYTE;
						break;
					}
				case READ_4_BYTE:
					{
						ColumnValue value(start_pos,(start_pos+(int)READ_4_BYTE-1));
						rows.vec_col_val.push_back(value);
						start_pos+=(int)READ_4_BYTE;
						break;
					}
				case READ_8_BYTE:
					{
						ColumnValue value(start_pos,(start_pos+(int)READ_8_BYTE-1));
						rows.vec_col_val.push_back(value);
						start_pos+=(int)READ_8_BYTE;
						break;
					}



			}
		}
		rows.vec_rows.push_back(rows.vec_col_val);
		if(header.event_size==start_pos)
		{
			rows.setCount(rows.vec_rows.size());
			break;
		}
	}

	

}

// getters

uint32_t RowEvent::getTimestamp()
{
	return  header.timestamp;
}
enum_event_type RowEvent::getType()
{
	return header.type; 
}
uint32_t RowEvent::getServerId()
{
	return header.server_id;
}
uint32_t RowEvent::getEventSize()
{
	return header.event_size;
}
uint32_t RowEvent::getLogPos()
{
	return header.log_pos;
}
uint16_t RowEvent::getFlagH()
{
	return header.flag; 
}
uint64_t RowEvent::getTableId()
{
	return table_id;
}
uint16_t RowEvent::getFlagPh()
{
	return flag;
}
uint64_t RowEvent::getColumnCount()
{
	return column_count;
}
int * RowEvent::getColumnPresentBitmap()
{
	return column_bitmap;
}
RowVector RowEvent::getRows()
{
	rows.setCurrentPos(0);
	return rows.vec_rows;
}

//setters

void RowEvent::setTableId(uint64_t value)
{
	table_id = value;
}
void RowEvent::setFlagPh(uint16_t value)
{
	flag = value;
}
void RowEvent::setColumnCount(uint64_t value)
{
	column_count = value;
}
void RowEvent::setColumnPresentBitmap(int * value)
{
	column_bitmap = value;
}
void RowEvent::setNullBitmap(bool * value)
{
	null_bitmap = value;
}

void RowEvent::setRowsCount(int value)
{
	rows.setCount(value);
}
