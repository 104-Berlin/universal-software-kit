#pragma once

#ifdef EWIN
    #ifdef EXPORT_EDITOR_EXTENSION
        #define E_EDEXAPI __declspec(dllexport)
    #else
        #define E_EDEXAPI __declspec(dllimport)
    #endif
#else
    #define E_EDEXAPI
#endif



#include "engine.h"
#include "prefix_shared.h"
#include "engine_extension.h"

#include "graphics_wrapper.h"
#include "graphics_renderer.h"

#include "ui/editor_ext_ui.h"
#include "ui/editor_ui_register.h"