#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;

struct TestImageResource
{
    int Width;
    int Height;
    int Channels;
    u8* Pixels;

    void FromBuffer(ESharedBuffer buffer)
    {
    }
};

TEST(Resources, BasicManager)
{
    EResource resource("Test");
    resource.SetID(1);
    resource.Load<TestImageResource>();
}