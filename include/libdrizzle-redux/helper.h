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

#ifndef T_HELPER_H
#include <src/t_helper.h>
#include <src/t_helper.cc>

/**
 * @brief Read a number of bytes as a specific type
 * @details The number of bytes can be less than the size of the datatype to
 * convert the bytes into
 *
 * @param pos the position from which to read
 * @param data array of bytes
 * @tparam U The datatype to read
 * @tparam V The number of bytes to read
 * @return A value of type U
 */
template <typename U, int V>
DRIZZLE_API
U drizzle_binlog_read_bytes(int pos, const unsigned char* data);
#endif // T_HELPER_H

typedef enum {
	LEN_ENC_STR = -1,
	READ_1_BYTE = 1,
	READ_2_BYTE = 2,
	READ_3_BYTE = 3,
	READ_4_BYTE = 4,
	READ_5_BYTE = 5,
	READ_8_BYTE = 8,
	NOT_FOUND   = 0
} drizzle_field_byte_t;

typedef enum {
	STRING = 1,
	INT = 2
} enum_col_type;

// DRIZZLE_API
// bool getNextBit(uint8_t& val);

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
