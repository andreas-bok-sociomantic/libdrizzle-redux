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

#define mask(__b) \
    ((uint32_t)(__b)==32 ? 0xffffffff : \
     ((uint32_t)(__b)==24 ? 0xffffff : \
      ((uint32_t)(__b)==16 ? 0xffff : \
       ((uint32_t)(__b)==8 ? 0xff : 0xffffffffffffffff ))))


template<typename U, int V>
U readBytes(int pos, const unsigned char* data)
{
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
