#pragma once

namespace Engine {

    namespace inter {

        void E_INTER_API PrintProperty(EProperty* prop);
        void E_INTER_API SetCurrentThreadName(const EString& name);

        EString E_INTER_API EncodeBase64(const u8* data, size_t dataLen);
        bool E_INTER_API DecodeBase64(const EString& base64, u8** data, size_t* dataLen);

    }

}