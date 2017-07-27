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

#include <src/t_helper.h>

using namespace std;

/**
 * @brief read raw bytes as a given numeric type
 * @details Generic function to read a numeric value from
 * raw bytes
 *
 * @param pos index into the byte array
 * @param data char array
 * @tparam U the target type for the bytes
 * @tparam V number of bytes to read
 * @return A value of type U
 */
template<typename U, int V>
U drizzle_binlog_read_bytes(int pos, const unsigned char* data)
{
    static_assert(std::is_integral<U>::value,
        "The target type must integral");

    if ((int)(sizeof(data)-pos)<V)
    {
        return numeric_limits<U>::max();
    }

    auto byte_size = sizeof(U);
    int i = 1;
    U value = ((U)data[pos] & mask(byte_size*8));
    while (i < (int) byte_size)
    {
        value =((U)data[pos+i]<<(i*8)|value);
        i++;
    }

    return value;
}
