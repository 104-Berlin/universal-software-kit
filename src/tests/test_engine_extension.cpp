#include <gtest/gtest.h>
#include <engine.h>
#include <engine_extension.h>

#define EXTENSION_NAME "Exmaple1-Extension"

using namespace Engine;

TEST(ExtensionTest, Basics)
{
}

TEST(ExtensionTest, LoadExtension)
{
    EExtensionManager extensionManager;
    extensionManager.LoadExtension("libExample1.so");
}