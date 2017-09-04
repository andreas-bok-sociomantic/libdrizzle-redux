/*
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2008-2013 Drizzle Developer Group
 * Copyright (C) 2012 Brian Aker (brian@tangent.org)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 *     * The names of its contributors may not be used to endorse or
 * promote products derived from this software without specific prior
 * written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "config.h"

#include "src/common.h"

const char *drizzle_strerror(const drizzle_return_t arg)
{
  switch (arg)
  {
  case DRIZZLE_RETURN_OK: return "DRIZZLE_RETURN_OK";
  case DRIZZLE_RETURN_IO_WAIT: return "DRIZZLE_RETURN_IO_WAIT";
  case DRIZZLE_RETURN_PAUSE: return "DRIZZLE_RETURN_PAUSE";
  case DRIZZLE_RETURN_ROW_BREAK: return "DRIZZLE_RETURN_ROW_BREAK";
  case DRIZZLE_RETURN_MEMORY: return "DRIZZLE_RETURN_MEMORY";
  case DRIZZLE_RETURN_ERRNO: return "DRIZZLE_RETURN_ERRNO";
  case DRIZZLE_RETURN_INTERNAL_ERROR: return "DRIZZLE_RETURN_INTERNAL_ERROR";
  case DRIZZLE_RETURN_GETADDRINFO: return "DRIZZLE_RETURN_GETADDRINFO";
  case DRIZZLE_RETURN_NOT_READY: return "DRIZZLE_RETURN_NOT_READY";
  case DRIZZLE_RETURN_BAD_PACKET_NUMBER: return "DRIZZLE_RETURN_BAD_PACKET_NUMBER";
  case DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET: return "DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET";
  case DRIZZLE_RETURN_BAD_PACKET: return "DRIZZLE_RETURN_BAD_PACKET";
  case DRIZZLE_RETURN_PROTOCOL_NOT_SUPPORTED: return "DRIZZLE_RETURN_PROTOCOL_NOT_SUPPORTED";
  case DRIZZLE_RETURN_UNEXPECTED_DATA: return "DRIZZLE_RETURN_UNEXPECTED_DATA";
  case DRIZZLE_RETURN_NO_SCRAMBLE: return "DRIZZLE_RETURN_NO_SCRAMBLE";
  case DRIZZLE_RETURN_AUTH_FAILED: return "DRIZZLE_RETURN_AUTH_FAILED";
  case DRIZZLE_RETURN_NULL_SIZE: return "DRIZZLE_RETURN_NULL_SIZE";
  case DRIZZLE_RETURN_ERROR_CODE: return "DRIZZLE_RETURN_ERROR_CODE";
  case DRIZZLE_RETURN_TOO_MANY_COLUMNS: return "DRIZZLE_RETURN_TOO_MANY_COLUMNS";
  case DRIZZLE_RETURN_ROW_END: return "DRIZZLE_RETURN_ROW_END";
  case DRIZZLE_RETURN_LOST_CONNECTION: return "DRIZZLE_RETURN_LOST_CONNECTION";
  case DRIZZLE_RETURN_COULD_NOT_CONNECT: return "DRIZZLE_RETURN_COULD_NOT_CONNECT";
  case DRIZZLE_RETURN_NO_ACTIVE_CONNECTIONS: return "DRIZZLE_RETURN_NO_ACTIVE_CONNECTIONS";
  case DRIZZLE_RETURN_HANDSHAKE_FAILED: return "DRIZZLE_RETURN_HANDSHAKE_FAILED";
  case DRIZZLE_RETURN_TIMEOUT: return "DRIZZLE_RETURN_TIMEOUT";
  case DRIZZLE_RETURN_INVALID_ARGUMENT: return "DRIZZLE_RETURN_INVALID_ARGUMENT";
  case DRIZZLE_RETURN_SSL_ERROR: return "DRIZZLE_RETURN_SSL_ERROR";
  case DRIZZLE_RETURN_EOF: return "DRIZZLE_RETURN_EOF";
  case DRIZZLE_RETURN_STMT_ERROR: return "DRIZZLE_RETURN_STMT_ERROR";
  case DRIZZLE_RETURN_BINLOG_CRC: return "DRIZZLE_RETURN_BINLOG_CRC";
  case DRIZZLE_RETURN_TRUNCATED: return "DRIZZLE_RETURN_TRUNCATED";
  case DRIZZLE_RETURN_INVALID_CONVERSION: return "DRIZZLE_RETURN_INVALID_CONVERSION";
  case DRIZZLE_RETURN_NOT_FOUND: return "DRIZZLE_RETURN_NOT_FOUND";
  case DRIZZLE_RETURN_ROW_REND: return "DRIZZLE_RETURN_ROW_REND";
  case DRIZZLE_RETURN_MAX: return "DRIZZLE_RETURN_MAX";
  default: return "DRIZZLE_RETURN_UNKNOWN_ERROR";
  }
}
