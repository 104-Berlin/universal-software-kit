#ifndef __PREFIX_INTERFACE_H
#define __PREFIX_INTERFACE_H


#ifdef EWIN
    #ifdef SH_INTERFACE_EXPORT
        #define E_INTER_API __declspec(dllexport)
    #else
        #define E_INTER_API __declspec(dllimport)
    #endif
#else
    #define E_INTER_API
#endif


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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#endif


#include "engine_extension.h"

#include "util/interface_util.h"

#include "socket/interface_socket.h"
#include "socket/interface_register_connection.h"
#include "socket/interface_register_socket.h"

#include "interface_functions.h"


#include "resource_types/interface_rt_txt.h"

#endif