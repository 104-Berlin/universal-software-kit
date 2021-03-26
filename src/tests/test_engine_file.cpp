#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

TEST(FileTest, CheckFileStrings)
{
    using namespace Engine;
    EFile file("test/TestFile.png");

    EXPECT_STREQ(file.GetFileExtension().c_str(), "png");
    EXPECT_STREQ(file.GetFileName().c_str(), "TestFile");
    EXPECT_STREQ(file.GetPath().c_str(), "test/TestFile.png");
}

