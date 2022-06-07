#pragma once

#include "engine.h"

/**
 *  CURRENT SOCKET INCLUDES FOR WIN AND UNIX
 */
#ifdef EWIN
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#define WIN32_LEAN_AND_MEAN


#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)

#ifdef SH_INTERFACE_EXPORT
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

#include <Windows.h>

#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#endif


#include "template/usefull_templates.h"

#include "string/util_string.h"
#include "logging/engine_logging.h"
#include "exception/engine_exception.h"

#include "file/engine_shared_buffer.h"
#include "file/engine_folder.h"
#include "file/engine_file.h"
#include "file/engine_file_collection.h"
#include "file/engine_base64.h"


#include "socket/util_socket.h"