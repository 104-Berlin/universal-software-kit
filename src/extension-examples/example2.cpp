#include "engine_extension.h"

using namespace Engine;

enum class ElectricalType
{
    GENERATOR,
    INPUT,
    OUTPUT,
    CONSUMER,
};


struct ElectricalConnection
{
    EString Name;
    ElectricalConnection* ConnectingTo;

    ///////
    ElectricalType Type;
};


struct ElectricalInfo
{
    double Capacity;
    double IDontKnow;
};

struct ElectricalComponent 
{
    ElectricalInfo MyInfo;
    EVector<ElectricalConnection> Connections;
};


EXT_ENTRY
{
    ERef<EEnumDescription> electricalTypeDsc = EMakeRef<EEnumDescription>("ElectricalType");
    electricalTypeDsc->AddOption("GENERATOR");
    electricalTypeDsc->AddOption("INPUT");
    electricalTypeDsc->AddOption("OUTPUT");
    electricalTypeDsc->AddOption("CONSUMER");

    ERef<EStructDescription> electricalInfoDsc = EMakeRef<EStructDescription>("ElectricalInfo");
    electricalInfoDsc->AddField("Capacity", DoubleDescription());
    electricalInfoDsc->AddField("IDontKnow", DoubleDescription());

    ERef<EStructDescription> electricalDescription = EMakeRef<EStructDescription>("Electrical");
    electricalDescription->AddField("Type", electricalTypeDsc);
    electricalDescription->AddField("InfoArray", EMakeRef<EArrayDescription>(electricalInfoDsc));

    ETypeRegister::get().RegisterItem(extensionName, electricalDescription);
}