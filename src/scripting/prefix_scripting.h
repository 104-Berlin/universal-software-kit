#pragma once

#ifdef EWIN
    #ifdef SCRIPTING_EXPORT
        #define E_SAPI __declspec(dllexport)
    #else
        #define E_SAPI __declspec(dllimport)
    #endif
#else
    #define E_SAPI
#endif

#include "engine.h"
#include "prefix_shared.h"

#include "engine_script_context.h"

#include "chai/engine_chai_context.h"