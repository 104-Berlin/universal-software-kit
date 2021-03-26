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

TEST(FileTest, FileCollection)
{
    const char* fileContent = "MyFileContent";
    const char* collectionPath = "intern/MyFile.mf";
    using namespace Engine;
    ESharedBuffer fileBuffer;
    fileBuffer.InitWith<char>((void*)fileContent, strlen(fileContent) + 1);

    EFileCollection fileCollection;
    fileCollection.AddFile(collectionPath, fileBuffer);

    ESharedBuffer collectionBuffer = fileCollection.GetCompleteBuffer();

    EFileCollection newFileCollection;
    newFileCollection.SetFromCompleteBuffer(collectionBuffer);

    ESharedBuffer newFileBuffer;
    bool foundFile = newFileCollection.GetFileAt(collectionPath, &newFileBuffer);
    EXPECT_TRUE(foundFile);
    EXPECT_EQ(fileBuffer.GetSizeInByte(), newFileBuffer.GetSizeInByte());
    EXPECT_EQ(fileBuffer.GetElementCount(), newFileBuffer.GetElementCount());
}

TEST(FileTest, FielSaveAndRead)
{
    const char* fileContent = "MyFileContent";
    const char* filePath = "MyFile.mf";
    const char* fileBufferPath = "MyFile.buf";
    using namespace Engine;

    ESharedBuffer fileBuffer;
    fileBuffer.InitWith<char>((void*)fileContent, strlen(fileContent) + 1);

    {
        EFile writeFile(filePath);
        writeFile.SetFileAsString(fileContent);
    }
    
    {
        EFile readFile(filePath);
        EXPECT_TRUE(readFile.Exist());
        EString readString = readFile.GetFileAsString();
        EXPECT_STREQ(readString.c_str(), fileContent);
    }

    {
        EFile writeFile(fileBufferPath);
        writeFile.SetFileBuffer(fileBuffer);
        writeFile.SaveBufferToDisk();
    }
    {
        EFile readFile(fileBufferPath);
        EXPECT_TRUE(readFile.Exist());
        readFile.LoadToMemory();
        ESharedBuffer readFileBuffer = readFile.GetBuffer();
        EXPECT_EQ(readFileBuffer.GetSizeInByte(), fileBuffer.GetSizeInByte());
    }
}