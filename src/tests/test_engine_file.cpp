#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

TEST(FileTest, CheckFileStrings)
{
    E_INFO("Log Something");
    using namespace Engine;
    EFile file(Path::Join("test", "TestFile.png"));

    EXPECT_STREQ(file.GetFileExtension().c_str(), "png");
    EXPECT_STREQ(file.GetFileName().c_str(), "TestFile");
    EXPECT_STREQ(file.GetPath().c_str(), Path::Join("test", "TestFile.png").c_str());
}

TEST(FileTest, FileCollection)
{
    EString fileContent = "MyFileContent";
    EString collectionPath = Engine::Path::Join("intern", "MyFile.mf");
    using namespace Engine;
    ESharedBuffer fileBuffer;
    fileBuffer.InitWith<char>((void*)fileContent.c_str(), fileContent.length() + 1);

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
    EString fileContent = "MyFileContent";
    EString filePath = "MyFile.mf";
    EString fileBufferPath = "MyFile.buf";
    using namespace Engine;

    ESharedBuffer fileBuffer;
    fileBuffer.InitWith<char>((void*)fileContent.c_str(), fileContent.length() + 1);

    {
        EFile writeFile(filePath);
        writeFile.SetFileAsString(fileContent);
    }
    
    {
        EFile readFile(filePath);
        EXPECT_TRUE(readFile.Exist());
        EString readString = readFile.GetFileAsString();
        EXPECT_STREQ(readString.c_str(), fileContent.c_str());
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