
#ifndef T_HELPER_H
#define T_HELPER_H

#include <limits>

template <typename U, int V = sizeof(U)> U
readBytes(int pos, const unsigned char* data);

#endif // T_HELPER_H
