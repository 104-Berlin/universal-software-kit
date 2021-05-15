#include "engine_extension.h"

using namespace Engine;

enum ElectricalType
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
    EStructDescription* electricalInfoDsc = new EStructDescription("ElectricalInfo");
    electricalInfoDsc->AddField("Capacity", DoubleDescription());
    electricalInfoDsc->AddField("IDontKnow", DoubleDescription());

    EStructDescription* electricalDescription = new EStructDescription("Electrical");
    electricalDescription->AddField("Info", electricalInfoDsc);
}