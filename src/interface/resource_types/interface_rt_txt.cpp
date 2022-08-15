#include "prefix_interface.h"


using namespace Engine;

void ETxtResource::FromBuffer(Engine::ESharedBuffer buffer)
{
    String = buffer.Data<char>();    
}
