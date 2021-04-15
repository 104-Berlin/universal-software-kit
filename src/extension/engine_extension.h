#pragma once


#ifdef EWIN
    #ifdef EXTENSION_EXPORT
        #define E_UIAPI __declspec(dllexport)
    #else
        #define E_UIAPI __declspec(dllimport)
    #endif
#else
    #define E_UIAPI
#endif



#include "engine.h"
#include "prefix_shared.h"

#include "engine_extension_register.h"

#include "ui/engine_ext_ui.h"
#include "ui/engine_ui_register.h"