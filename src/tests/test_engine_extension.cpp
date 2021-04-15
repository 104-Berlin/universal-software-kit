#include <gtest/gtest.h>
#include <engine.h>
#include <engine_extension.h>

#define EXTENSION_NAME "Exmaple1-Extension"

using namespace Engine;

TEST(ExtensionTest, Basics)
{
    EUIRegister uiRegister;

    ERef<EUIPanel> uiPanel = EMakeRef<EUIPanel>("Testing Panel");


    uiRegister.RegisterItem(EXTENSION_NAME, uiPanel);
}