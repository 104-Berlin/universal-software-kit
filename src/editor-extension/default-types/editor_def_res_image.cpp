#include "editor_extension.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace Editor;
using namespace Engine;


EResourceDescription::ResBuffer ResImage::ImportImage(const EResourceDescription::RawBuffer data)
{
    EResourceDescription::ResBuffer result;

    int x, y, n;
    stbi_uc* imageData = stbi_load_from_memory(data.Data, data.Size, &x, &y, &n, 0);

    result.Data = new u8[x * y * n];
    memcpy(result.Data, (u8*)imageData, x * y * n);
    result.Size = x * y * n;

    stbi_image_free(imageData);

    EImageUserData* userData = new EImageUserData;
    userData->width = x;
    userData->height = y;
    userData->channels = n;

    result.UserData = (u8*) userData;
    result.UserDataSize = sizeof(EImageUserData);

    return result;
}