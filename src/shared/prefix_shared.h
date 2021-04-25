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



#include "macros/engine_basic_macros.h"


#include "file/engine_shared_buffer.h"
#include "file/engine_folder.h"
#include "file/engine_file.h"
#include "file/engine_file_collection.h"

#include "logging/engine_logging.h"

#include "event/engine_event_dispatcher.h"
#include "resource/engine_resource_manager.h"



#include "register/entt/entt.hpp"
#include "register/engine_scene.h"