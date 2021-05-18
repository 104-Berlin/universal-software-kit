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
    EEnumDescription* electricalTypeDsc = new EEnumDescription("ElectricalType");
    electricalTypeDsc->AddOption("GENERATOR");
    electricalTypeDsc->AddOption("INPUT");
    electricalTypeDsc->AddOption("OUTPUT");
    electricalTypeDsc->AddOption("CONSUMER");

    EStructDescription* electricalInfoDsc = new EStructDescription("ElectricalInfo");
    electricalInfoDsc->AddField("Capacity", DoubleDescription());
    electricalInfoDsc->AddField("IDontKnow", DoubleDescription());

    EArrayDescription* someArrayDsc = new EArrayDescription(electricalInfoDsc);

    EStructDescription* electricalDescription = new EStructDescription("Electrical");
    electricalDescription->AddField("Type", electricalTypeDsc);
    electricalDescription->AddField("InfoArray", someArrayDsc);
}