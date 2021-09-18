#include "prefix_interface.h"

using namespace Engine;

ERegisterSocket::ERegisterSocket(int port) 
    : fPort(port)
{
    
}

ERegisterSocket::~ERegisterSocket() 
{
#ifdef EWIN
    
#else
#endif
}
