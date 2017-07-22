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

#include <libdrizzle/t_helper.h>
using namespace std;

template<typename U, int V = sizeof(U)>
U readBytes(int pos, const unsigned char* data)
{
    if ((int)(sizeof(data)-pos)<V)
    {
        return numeric_limits<U>::max();
    }

    auto byte_size = sizeof(U);
    uint i = 1;
    U value = ((U)data[pos] & mask(byte_size*8));
    while (i < byte_size)
    {
        value =((U)data[pos+i]<<(i*8)|value);
        i++;
    }

    return value;
}
