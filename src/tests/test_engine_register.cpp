#include <gtest/gtest.h>
#include <engine.h>
#include <prefix_shared.h>

using namespace Engine;

template <typename Fn>
void Loop(void* data, ETypeDescription typeDescription, Fn&& callback)
{
    if (typeDescription.fType == EType::STRUCT)
    {
        for(const ETypeDescription& dsc : typeDescription.fChilds)
        {
            byte* currentPtr = (byte*)data;
            currentPtr += dsc.fOffset;
            switch (dsc.fType)
            {
                case EType::INT: 
                {
                    int intData = *(int*)currentPtr;
                    callback(dsc.fName, intData);
                    break;
                }
                case EType::FLOAT:
                {
                    float floatData = *(float*)currentPtr;
                    callback(dsc.fName, floatData);
                    break;
                }
                case EType::STRING:
                {
                    EString stringData = *(EString*)currentPtr;
                    callback(dsc.fName, stringData);
                    break;
                }
                case EType::STRUCT:
                {
                    Loop(currentPtr, dsc, callback);
                    break;
                }
            }
        }
    }
}


E_STRUCTURE(MyComponent, 
    (int) MyInteger,
    (EString) MyString
)


TEST(RegisterTest, Basics)
{
    MyComponent myCompIstance;
    myCompIstance.MyInteger = 32;
    myCompIstance.MyString = "Hello World";

    Loop(&myCompIstance, E_DESC(MyComponent), [](EString name, auto value) {
        std::cout << name << ": " << value << std::endl;
    });
}