#pragma once


#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"

#include "graphics/editor_buffer.h"
#include "graphics/editor_camera.h"
#include "graphics/editor_light.h"
#include "graphics/editor_render_command_queue.h"
#include "graphics/editor_render_context.h"
#include "graphics/editor_shader_uniforms.h"
#include "graphics/editor_shader.h"
#include "graphics/editor_texture.h"
#include "graphics/editor_renderer.h"
#include "graphics/editor_ui_renderer.h"


#include "platform/editor_platform.h"

#include "platform/opengl/editor_opengl_buffer.h"
#include "platform/opengl/editor_opengl_render_context.h"
#include "platform/opengl/editor_opengl_shader.h"
#include "platform/opengl/editor_opengl_texture.h"


#include "editor_application.h"