#include "engine.h"
#include <logging/engine_error.h>

void TestFunc(int x)
{
    if (x > 10)
    {
        throw(Engine::EError("Integer to big"));   
    }
}

int main()
{
    try
    {
        TestFunc(100);
    }
    catch(Engine::EError e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}