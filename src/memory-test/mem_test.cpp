#include "prefix_shared.h"

using namespace Engine;

static constexpr u32 StorageCount = 1;

// 16 Byte memory
E_STORAGE_STRUCT(BasicType_16, 
    (i32, Val1),
    (i32, Val2),
    (i32, Val3),
    (i32, Val4)
)

struct TypeIdentifier
{
    const char* ID;
    const char* Namespace;
};

class TypeDescription
{
private:
    const char* fID;
    EVector<TypeIdentifier> fChildren;
public:
    TypeDescription(const char* id) : fID(id) {}
    ~TypeDescription() {}

    const char* GetID() const { return fID; }

    void AddChild(TypeIdentifier identifier)
    {
        fChildren.push_back(identifier);
    }
};

class TypeContext
{
private:
    EUnorderedMap<EString, TypeDescription*> fRegisteredTypes;
public:
    TypeContext()
    {
        // Register Default types
        // Float, Double, Int, Uint, String, Boolean
        RegisterType(new TypeDescription("int"));
        RegisterType(new TypeDescription("uint"));
        RegisterType(new TypeDescription("float"));
        RegisterType(new TypeDescription("double"));
        RegisterType(new TypeDescription("string"));
        RegisterType(new TypeDescription("bool"));
    }

    ~TypeContext()
    {
        Clear();
    }

    void Clear()
    {
        for (auto& entry : fRegisteredTypes)
        {
            delete entry.second;
        }
        fRegisteredTypes.clear();
    }

    void RegisterType(TypeDescription* dsc, const char* namespac = "")
    {
        if (!dsc)
        {
            return;
        }
        EString namespaceName = GetNamespaceName({dsc->GetID(), namespac});

        fRegisteredTypes.insert({namespaceName, dsc});
        printf("Registered Type: %s\n", namespaceName);
    }

    static EString GetNamespaceName(TypeIdentifier identifier)
    {
        return EString(identifier.Namespace) + (strlen(identifier.Namespace) > 0 ? "@" : "") + identifier.ID;
    }

    bool HasType(TypeIdentifier identifier) const
    {
        EString namespaceName = GetNamespaceName(identifier);

        return fRegisteredTypes.find(namespaceName) != fRegisteredTypes.end();
    }

    TypeDescription* GetTypeFromIdentifier(TypeIdentifier identifier) const
    {
        EString namespaceName = GetNamespaceName(identifier);

        if (!HasType(identifier))
        {
            return nullptr;
        }
    }

    const char* FindIdentifier(TypeDescription* dsc) const
    {
        for (auto& entry : fRegisteredTypes)
        {
            if (entry.second == dsc)
            {
                return entry.first.c_str();
            }
        }
        return "";
    }
};

int main()
{
    TypeContext context;
    TypeDescription* myDsc = new TypeDescription("MyStruct");
    myDsc->AddChild("int");
    myDsc->AddChild("int");
    myDsc->AddChild("int");

    EDataBase dataBase;

    for (u32 i = 0; i < StorageCount; i++)
    {
        EDataBase::Entity entity = dataBase.CreateEntity();
        dataBase.AddComponent<BasicType_16>(entity);
    }

    system("PAUSE");

    return 0;
}