#include "prefix_interface.h"

using namespace Engine;

E_STORAGE_STRUCT(CommandType,
    (EString, StringValue),
    (double, SomeDouble)
)

int main(int argc, char** argv)
{  
    shared::StaticSharedContext::Start();
    
    shared::StaticSharedContext::instance().GetExtensionManager().GetTypeRegister().RegisterItem("CommandLine", CommandType::_dsc);

    EString input;
    do
    {
        /* code */
        std::getline(std::cin, input);
        EVector<EString> args = EStringUtil::SplitString(input, " ");

        if (args.size() > 0)
        {
            if (args[0] == "New")
            {
                shared::CreateEntity();
            }
            else if (args[0] == "Add")
            {
                if (args.size() == 3)
                {
                    ERegister::Entity entity = std::stoi(args[1]);
                    shared::CreateComponent(args[2], entity);
                }
                else
                {
                    std::cout << "Invalid use! Add <entityid> <valueid>" << std::endl;
                }
            }
        }
    } while (input != "Q");
    
    shared::StaticSharedContext::CleanUp();
    return 0;
}