#pragma once

#include "glm/glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm/gtx/quaternion.hpp"


typedef glm::vec2 EVec2;
typedef glm::vec3 EVec3;
typedef glm::vec4 EVec4;
typedef glm::vec4 EColor;
typedef glm::mat4 EMat4;

#include "file/engine_shared_buffer.h"
#include "file/engine_folder.h"
#include "file/engine_file.h"
#include "file/engine_file_collection.h"

#include "logging/engine_logging.h"

#include "register/engine_data_handle.h"
#include "register/engine_descriptor.h"



#include "event/engine_event_dispatcher.h"