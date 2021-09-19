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
            auto& entry = args[0];
            std::transform(entry.begin(), entry.end(), entry.begin(),
                            [](unsigned char c){ return std::tolower(c); });
            if (args[0] == "new")
            {
                shared::CreateEntity();
            }
            else if (args[0] == "add")
            {
                if (args.size() == 3)
                {
                    ERegister::Entity entity = std::stoi(args[1]);
                    shared::CreateComponent(args[2], entity);
                }
                else
                {
                    std::cout << "Invalid use! add <entityid> <valueid>" << std::endl;
                }
            }
            else if (args[0] == "get")
            {
                if (args.size() == 3)
                {
                    ERegister::Entity entity = std::stoi(args[1]);
                    ERef<EProperty> component = shared::GetValue(entity, args[2]);
                    if (component)
                    {
                        inter::PrintProperty(component.get());
                    }
                }
                else
                {
                    std::cout << "Invalid use! show <entityid> <valueid>" << std::endl;
                }
            }
            else if (args[0] == "set")
            {
                if (args.size() == 4)
                {
                    ERegister::Entity entity = std::stoi(args[1]);
                    
                    shared::SetValue(entity, args[2], args[3]);
                }
                else
                {
                    std::cout << "Invalid use! set <entityid> <valueident> <value>" << std::endl;
                }
            }
        }
    } while (input != "Q");
    
    shared::StaticSharedContext::Stop();
    return 0;
}