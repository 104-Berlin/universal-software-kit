#pragma once


#ifdef EWIN
    #ifdef SHARED_EXPORT
        #define E_API __declspec(dllexport)
    #else
        #define E_API __declspec(dllimport)
    #endif
#else
    #define E_API
#endif

//#include "../deps/chai/include/chaiscript/chaiscript.hpp"

#include "prefix_util.h"

#include "macros/engine_basic_macros.h"
#include "macros/engine_storage_macro.h"


#include "storage/engine_storage_description.h"
#include "storage/engine_storage_values.h"

#include "event/engine_event_dispatcher.h"
#include "resource/engine_resource_manager.h"
#include "resource/engine_resource_storage_link.h"



#include "register/engine_register.h"
#include "register/engine_serializer.h"
#include "register/engine_deserializer.h"
