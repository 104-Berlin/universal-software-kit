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
    EValueDescription _electricalTypeDsc = EValueDescription(EValueType::ENUM, "ElectricalType");
    _electricalTypeDsc
        .AddEnumOption("GENERATOR")
        .AddEnumOption("INPUT")
        .AddEnumOption("OUTPUT")
        .AddEnumOption("CONSUMER");

    EValueDescription _electricalInfoDsc = EValueDescription(EValueType::STRUCT, "ElectricalInfo");
    _electricalInfoDsc
        .AddStructField("Capacity", DoubleDescription)
        .AddStructField("IDontKnow", DoubleDescription);

    EValueDescription _electricalDescription = EValueDescription(EValueType::STRUCT, "Electrical");
    _electricalDescription
        .AddStructField("Type", _electricalTypeDsc)
        .AddStructField("Type2", _electricalTypeDsc)
        .AddStructField("InfoArray", _electricalInfoDsc.GetAsArray());


    info.GetTypeRegister().RegisterItem(extensionName, _electricalDescription);
}