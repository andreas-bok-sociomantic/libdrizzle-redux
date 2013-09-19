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

#include "config.h"
#include<iostream>
#include "libdrizzle/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include<string.h>
#include<limits.h>
#ifndef row_event
#define row_event

#include<libdrizzle-5.1/row_event.h>

#endif

#ifndef HELPER
#define HELPER

#include<libdrizzle-5.1/helper.h>

#endif

using namespace std;
using namespace binlogevent;



void RowEvent::initWithData(const unsigned char* data)
{
	if(row_event_type<0)  // Event type not exist
		return;
	uint64_t tmp;
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

	tmp_int = getEncodedLen(start_pos,data);
	if(tmp_int==0)
		return;
	setColumnCount(tmp_int); // start_pos will also get updated

	int size= (column_count+7)/8; // length of present column bitmap1
	
	int count_cnp=0;  // count of column not present

	bool tmp_present[column_count]; // bit array of column bitmap1
	count_cnp = getBoolArray(tmp_present,data,start_pos,size,column_count); // return -1 when data problem 

	if(count_cnp==-1)
		return;
	
	setColumnPresentBitmap(tmp_present);
	
	start_pos+=size;	//null-bitmap
	size= (column_count-count_cnp+7)/8; // length of null bitmap in bytes 

	bool tmp_bool[column_count-count_cnp];
	count_cnp = getBoolArray(tmp_bool,data,start_pos,size,(column_count-count_cnp)); // return -1 when data problem 
	
	if(count_cnp==-1)
		return;
	
	setNullBitmap(tmp_bool);

	start_pos+=size;

	//LOOP for ROWs
	while(1)
	{
		rows.vec_col_val.clear();
		string str_col_val; // column value as string
		unsigned int    int_col_val = UINT_MAX; // column value as int
		for(uint64_t val=0;val<column_count;val++)
		{
			if(column_bitmap[val]==0)
				continue;

			if(null_bitmap[val]==1)
			{
				str_col_val.clear();
				ColumnValue value(str_col_val,1,val); // 1 -> null, val is column number
				rows.vec_col_val.push_back(value);
				continue;

			}

			enum_field_bytes num;
			num = lookup_field_bytes((enum_field_types)column_type[val]);
			switch(num)
			{
				case LEN_ENC_STR:
					{
						int len;
						len=getEncodedLen(start_pos,data);
						if(len==0)
						{
							return;
						}
						str_col_val.clear();
						len=start_pos+len-1;

						if((int)sizeof(data)-start_pos<len)
							return;
						for(int it=start_pos;it<=len;it++) // Formation of  string
						{
							str_col_val.push_back(data[it]);
						}
						ColumnValue value(str_col_val,0,val);  // 0 -> not null
						rows.vec_col_val.push_back(value);
						start_pos+=len; //length of string
						break;
					}
				case READ_1_BYTE:
					{
						if((int)sizeof(data)-start_pos<1)
							return;
						str_col_val.clear();
						int_col_val = (uint8_t)data[start_pos];
						str_col_val = getIntToStr(int_col_val);
						ColumnValue value(str_col_val,0,val);
						rows.vec_col_val.push_back(value);
						start_pos+=(int)READ_1_BYTE;
						break;
					}
				case READ_2_BYTE:
					{
						str_col_val.clear();
						int_col_val = getByte2(start_pos,data);
						if(int_col_val==USHRT_MAX)
							return;
						str_col_val = getIntToStr(int_col_val);
						ColumnValue value(str_col_val,0,val);
						rows.vec_col_val.push_back(value);
						start_pos+=(int)READ_2_BYTE;
						break;
					}
				case READ_4_BYTE:
					{
						str_col_val.clear();
						int_col_val = getByte4(start_pos,data);
						if(int_col_val==UINT_MAX)
							return;
						str_col_val = getIntToStr(int_col_val);
						ColumnValue value(str_col_val,0,val);
						rows.vec_col_val.push_back(value);
						start_pos+=(int)READ_4_BYTE;
						break;
					}
				case READ_8_BYTE:
					{
						str_col_val.clear();
						int_col_val = getByte8(start_pos,data);
						if(int_col_val==UINT_MAX)
							return;
						str_col_val = getIntToStr(int_col_val);
						ColumnValue value(str_col_val,0,val);
						rows.vec_col_val.push_back(value);
						start_pos+=(int)READ_8_BYTE;
						break;
					}
				case NOT_FOUND:
					{
						break;
					}



			}
		}
		
		rows.vec_rows.push_back(rows.vec_col_val);
		if((int)header.event_size==start_pos)
		{
			rows.setCount(rows.vec_rows.size());
			break;
		}
	}

	

}

// getters
int RowEvent::getRowEventType()
{
	        return row_event_type;
}
uint32_t RowEvent::getTimestamp()
{
	return  header.timestamp;
}
enum_event_type RowEvent::getType()
{
	return (enum_event_type)header.type; 
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
bool * RowEvent::getColumnPresentBitmap()
{
	return column_bitmap;
}
RowVector RowEvent::getRows()
{
	rows.setCurrentPos(0);
	return rows.vec_rows;
}

/*Row RowEvent::getRow()
{
	rows.setCurrentPos(0);
	return rows.vec_rows;
}

RowVector RowEvent::getAllRows()
{
	rows.setCurrentPos(0);
	return rows.vec_rows;
}*/



//setters
//setters
void RowEvent::setRowEventType(int value)
{
	        row_event_type=value;
}
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
void RowEvent::setColumnPresentBitmap(bool * value)
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
