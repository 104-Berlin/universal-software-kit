// This converts any file to a c byte buffer
#include "prefix_util.h"

using namespace Engine;

int main(int argc, char** argv)
{
    EString file;
    if (argc <= 1) 
    { 
        std::getline(std::cin, file);
    }
    else
    {
        file = argv[1];
    }

    EFile bufferFile(file);
    if (!bufferFile.Exist())
    {
        E_ERROR(EString("Could not find source file \"") + argv[1] + "\"");
        return EXIT_FAILURE;
    }
    EString fullPath = bufferFile.GetFullPath();
    std::cout << "Reading file " << fullPath << std::endl;

    EString saveToPath = fullPath.substr(0, fullPath.length() - bufferFile.GetFileExtension().length()) + "h";
    if (argc == 3) // To File
    {
        saveToPath = argv[2];
    }

    bufferFile.LoadToMemory();
    ESharedBuffer fileBuffer = bufferFile.GetBuffer();
    if (fileBuffer.IsNull())
    {
        E_ERROR("Could not read the content of the file!");
        return EXIT_FAILURE;
    }
    EString name = bufferFile.GetFileName();
    std::replace(name.begin(), name.end(), ' ', '_');
    std::replace(name.begin(), name.end(), '-', '_');


    u8* data = fileBuffer.Data<u8>();
    std::stringstream textToSave;
    textToSave << "static size_t " << name << "_size = " << std::to_string(fileBuffer.GetSizeInByte()) << ";";
    textToSave << "\n";
    textToSave << "static const unsigned char " << name << "_buffer[] = {";
    textToSave << std::hex << std::setfill('0');
    for (size_t i = 0; i < fileBuffer.GetSizeInByte(); i++)
    {
        textToSave << "0x";
        textToSave << std::setw(2) << static_cast<int>(data[i]);
        if (i < fileBuffer.GetSizeInByte() - 1)
        {
            textToSave << ", ";
        }
    }
    textToSave << "};";

    EFile saveToFile(saveToPath);
    saveToFile.SetFileAsString(textToSave.str());
    

    return EXIT_SUCCESS;
}