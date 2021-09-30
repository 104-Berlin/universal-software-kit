#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;

const EVector<u8> bufferA = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
const EVector<u8> bufferB = {0xf1, 0xff};

void TestBase64(const EVector<u8>& buffer)
{    
    EString base64 = Base64::Encode(buffer.data(), buffer.size());

    u8* decodeBuffer;
    size_t decodeLength;
    bool couldDecode = Base64::Decode(base64, &decodeBuffer, &decodeLength);

    EXPECT_TRUE(couldDecode);
    if (couldDecode)
    {
        EXPECT_EQ(decodeLength, buffer.size());
        for (size_t i = 0; i < decodeLength; i++)
        {
            EXPECT_EQ(decodeBuffer[i], buffer[i]);
        }
    }
}

TEST(Base64, Encoding_Decoding)
{
    TestBase64(bufferA);
    TestBase64(bufferB);
}