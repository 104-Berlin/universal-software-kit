#pragma once

namespace Engine {

    namespace Base64 {
        EString Encode(const u8* data, size_t dataLen);

        /**
         * @brief Decondes a Base64 string to raw data. 
         * @return If returned true you have to release the data with delete[] data;
         */
        bool Decode(const EString& base64, u8** data, size_t* dataLen);
    }

}