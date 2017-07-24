#pragma once

#include <limits>
#include <cstdint>
#include <type_traits>

template <typename U, int V = sizeof(U)> U
readBytes(int pos, const unsigned char* data);
