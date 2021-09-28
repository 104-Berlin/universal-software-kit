#pragma once

namespace Engine {

    namespace Base64 {
        EString Encode(const u8* data, size_t dataLen);
        bool Decode(const EString& base64, u8** data, size_t* dataLen);
    }

}