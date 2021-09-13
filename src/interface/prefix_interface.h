#pragma once


#ifdef EWIN
    #ifdef SH_INTERFACE_EXPORT
        #define E_INTER_API __declspec(dllexport)
    #else
        #define E_INTER_API __declspec(dllimport)
    #endif
#else
    #define E_INTER_API
#endif


#include "engine_extension.h"

#include "util/interface_util.h"

#include "interface_functions.h"