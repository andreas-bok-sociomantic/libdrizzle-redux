/*
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2008 Eric Day (eday@oddments.org)
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

#pragma once

/**
 * @file
 * @brief Drizzle Declarations
 */

#include <sys/types.h>

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN

# include <Windows.h>
# include <winsock2.h>
# include <ws2tcpip.h>
# include <io.h>

# undef close
# define close _close
typedef unsigned int in_port_t;
typedef long ssize_t;

# define snprintf _snprintf
# define inline __inline

struct sockaddr_un
{
  short int sun_family;
  char sun_path[108];
};

# define poll WSAPoll
//# define pollfd WSAPOLLFD

#if defined(__GNUC__)
# include <stdbool.h>
#else
# if !defined(__cplusplus)
typedef enum { false = 0, true = 1 } _Bool;
typedef _Bool bool;
#endif 
#endif

#else
# if !defined(__cplusplus)
#  include <stdbool.h>
# endif
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/un.h>
# include <netdb.h>
# include <poll.h>
#endif
#ifdef USE_OPENSSL
#include <openssl/ssl.h>
#endif

#include <libdrizzle-5.0/visibility.h>
#include <libdrizzle-5.0/constants.h>
#include <libdrizzle-5.0/structs.h>
#include <libdrizzle-5.0/conn.h>
#include <libdrizzle-5.0/result.h>
#include <libdrizzle-5.0/column.h>

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @addtogroup drizzle Drizzle Declarations
 * @ingroup drizzle_client_interface
 * @ingroup drizzle_server_interface
 *
 * This is the core library structure that other structures (such as
 * connections) are created from.
 *
 * There is no locking within a single drizzle_st structure, so for threaded
 * applications you must either ensure isolation in the application or use
 * multiple drizzle_st structures (for example, one for each thread).
 * @{
 */

/**
 * Intialize the Drizzle library
 *
 * Currently only initalizes the SSL library
 */
DRIZZLE_API
void drizzle_library_init(void);

DRIZZLE_API
void drizzle_library_deinit(void);

/**
 * Get library version string.
 *
 * @return Pointer to static buffer in library that holds the version string.
 */
DRIZZLE_API
const char *drizzle_version(void);

/**
 * Get bug report URL.
 *
 * @return Bug report URL string.
 */
DRIZZLE_API
const char *drizzle_bugreport(void);

/**
 * Get string with the name of the given verbose level.
 *
 * @param[in] verbose Verbose logging level.
 * @return String form of verbose level.
 */
DRIZZLE_API
const char *drizzle_verbose_name(drizzle_verbose_t verbose);

/**
 * Get current socket I/O activity timeout value.
 *
 * @param[in] drizzle Drizzle structure previously initialized with
 *  drizzle_create() or drizzle_clone().
 * @return Timeout in milliseconds to wait for I/O activity. A negative value
 *  means an infinite timeout.
 */
DRIZZLE_API
int drizzle_con_timeout(const drizzle_con_st *con);

/**
 * Set socket I/O activity timeout for connections in a Drizzle structure.
 *
 * @param[in] drizzle Drizzle structure previously initialized with
 *  drizzle_create() or drizzle_clone().
 * @param[in] timeout Milliseconds to wait for I/O activity. A negative value
 *  means an infinite timeout.
 */
DRIZZLE_API
void drizzle_con_set_timeout(drizzle_con_st *con, int timeout);

/**
 * Get current verbosity threshold for logging messages.
 *
 * @param[in] drizzle Drizzle structure previously initialized with
 *  drizzle_create() or drizzle_clone().
 * @return Current verbosity threshold.
 */
DRIZZLE_API
drizzle_verbose_t drizzle_con_verbose(const drizzle_con_st *con);

/**
 * Set verbosity threshold for logging messages. If this is set above
 * DRIZZLE_VERBOSE_NEVER and the drizzle_set_log_fn() callback is set to NULL,
 * messages are printed to STDOUT.
 *
 * @param[in] drizzle Drizzle structure previously initialized with
 *  drizzle_create() or drizzle_clone().
 * @param[in] verbose Verbosity threshold of what to log.
 */
DRIZZLE_API
void drizzle_con_set_verbose(drizzle_con_st *con, drizzle_verbose_t verbose);

/**
 * Set logging function for a drizzle structure. This function is only called
 * for log messages that are above the verbosity threshold set with
 * drizzle_set_verbose().
 *
 * @param[in] drizzle Drizzle structure previously initialized with
 *  drizzle_create() or drizzle_clone().
 * @param[in] function Function to call when there is a logging message.
 * @param[in] context Argument to pass into the callback function.
 */
DRIZZLE_API
void drizzle_con_set_log_fn(drizzle_con_st *con, drizzle_log_fn *function,
                        void *context);

/**
 * Initialize a connection structure. Always check the return value even if
 * passing in a pre-allocated structure. Some other initialization may have
 * failed.
 *
 * @param[in] drizzle Drizzle structure previously initialized with
 *  drizzle_create() or drizzle_clone().
 * @param[in] con Caller allocated structure, or NULL to allocate one.
 * @return On success, a pointer to the (possibly allocated) structure. On
 *  failure this will be NULL.
 */
DRIZZLE_LOCAL
drizzle_con_st *drizzle_con_create(void);

/**
 * Free a connection structure.
 *
 * @param[in] con Connection structure previously initialized with
 *  drizzle_con_create(), drizzle_con_clone(), or related functions.
 */
DRIZZLE_LOCAL
void drizzle_con_free(drizzle_con_st *con);

/**
 * Wait for I/O on connections.
 *
 * @param[in] drizzle Drizzle structure previously initialized with
 *  drizzle_create() or drizzle_clone().
 * @return Standard drizzle return value.
 */
DRIZZLE_API
drizzle_return_t drizzle_con_wait(drizzle_con_st *con);

/**
 * Get next connection that is ready for I/O.
 *
 * @param[in] drizzle Drizzle structure previously initialized with
 *  drizzle_create() or drizzle_clone().
 * @return Connection that is ready for I/O, or NULL if there are none.
 */
DRIZZLE_API
drizzle_con_st *drizzle_con_ready(drizzle_con_st *con);

/** @} */

#ifdef  __cplusplus
}
#endif