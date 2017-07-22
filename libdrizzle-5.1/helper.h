/*
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 *
 */

#pragma once

#include <limits>

#ifndef HELPERS
#define HELPERS

typedef enum{
	LEN_ENC_STR = -1,
	READ_1_BYTE = 1,
	READ_2_BYTE = 2,
	READ_3_BYTE = 3,
	READ_4_BYTE = 4,
	READ_5_BYTE = 5,
	READ_8_BYTE = 8,
	NOT_FOUND   = 0
} drizzle_field_byte_t;

typedef enum{
	STRING = 1,
	INT = 2
}enum_col_type;

#define bytes_col_count(__b) \
	((uint64_t)(__b)<0xfb ? 1 : \
	 ((uint64_t)(__b)==0xfc ? 2 : \
	  ((uint64_t)(__b)==0xfd ? 3 : 8)))

DRIZZLE_API
bool getNextBit(uint8_t& val);
/** gets the string of specified length
  *
  * @param[in] pos Start reading from pos.
  * @param[in] len Length of string
  * @param[in] data Raw data from binglog.
  */
 DRIZZLE_API
char * getString(int pos,int len,const unsigned char * data);

 DRIZZLE_API
uint64_t getEncodedLen(int& pos,const unsigned char * data);

 DRIZZLE_API
int lookup_metadata_field_size(drizzle_field_type_t field_type);

 DRIZZLE_API
std::string getIntToStr(uint64_t num);

 DRIZZLE_API
int getBoolArray(bool arr[],const unsigned char data[],int start_pos,int _byte,int _bit);

 DRIZZLE_API
drizzle_field_byte_t lookup_field_bytes(drizzle_field_type_t field_type);

#ifndef T_HELPER_H
#include <libdrizzle/t_helper.h>
#include <libdrizzle/t_helper.cc>
template <typename U, int V = sizeof(U)> U
DRIZZLE_API
readBytes(int pos, const unsigned char* data);
#endif // T_HELPER_H

#endif
