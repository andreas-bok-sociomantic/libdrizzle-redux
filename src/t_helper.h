#pragma once

#include <limits>
#include <cstdint>

template <typename U, int V = sizeof(U)> U
readBytes(int pos, const unsigned char* data);
