#pragma once

namespace Engine {

    enum class EType
    {
        INT,
        FLOAT,
        STRING,

        //ARRAY,
        //ENUM,
        STRUCT
    };

    struct ETypeDescription
    {
        EType                       fType;
        EString                     fName;
        size_t                      fOffset;
        size_t                      fSize;
        EVector<ETypeDescription>   fChilds;

        ETypeDescription(EType type, const EString& name, size_t offset, size_t size, const EVector<ETypeDescription>& childs = {})
            : fType(type), fName(name), fOffset(offset), fSize(size), fChilds(childs)
            {}
    };

}

#define E_DEFINE_STRUCT_VAR(typename) E_PAIR(typename);

#define E_DEFINE_STRUCT_ARG1(param) E_DEFINE_STRUCT_VAR(param)
#define E_DEFINE_STRUCT_ARG2(param, ...) E_DEFINE_STRUCT_VAR(param)\
                                            E_DEFINE_STRUCT_ARG1(__VA_ARGS__)
#define E_DEFINE_STRUCT_ARG3(param, ...) E_DEFINE_STRUCT_VAR(param)\
                                            E_DEFINE_STRUCT_ARG2(__VA_ARGS__)
#define E_DEFINE_STRUCT_ARG4(param, ...) E_DEFINE_STRUCT_VAR(param)\
                                            E_DEFINE_STRUCT_ARG3(__VA_ARGS__)






#define E_DEFINE_STRUCT_DESC_VAR_int(structname, name) Engine::ETypeDescription(Engine::EType::INT, E_STRINGIFY(name), offsetof(structname, name), sizeof(int))
#define E_DEFINE_STRUCT_DESC_VAR_EString(structname, name) Engine::ETypeDescription(Engine::EType::STRING, E_STRINGIFY(name), offsetof(structname, name), sizeof(EString))


#define E_DEFINE_STRUCT_DESC_VAR(structname, typename) E_CONCATENATE(E_DEFINE_STRUCT_DESC_VAR_, E_TYPEOF(typename))(structname, E_STRIP(typename))

#define E_DEFINE_STRUCT_DESC_ARG1(structname, param, ...) E_DEFINE_STRUCT_DESC_VAR(structname, param)
#define E_DEFINE_STRUCT_DESC_ARG2(structname, param, ...) E_DEFINE_STRUCT_DESC_VAR(structname, param),\
                                                            E_DEFINE_STRUCT_DESC_ARG1(structname, __VA_ARGS__)
#define E_DEFINE_STRUCT_DESC_ARG3(structname, param, ...) E_DEFINE_STRUCT_DESC_VAR(structname, param),\
                                                            E_DEFINE_STRUCT_DESC_ARG2(structname, __VA_ARGS__)
#define E_DEFINE_STRUCT_DESC_ARG4(structname, param, ...) E_DEFINE_STRUCT_DESC_VAR(structname, param),\
                                                            E_DEFINE_STRUCT_DESC_ARG3(structname, __VA_ARGS__)
#define E_DEFINE_STRUCT_DESC_ARG5(structname, param, ...) E_DEFINE_STRUCT_DESC_VAR(structname, param),\
                                                            E_DEFINE_STRUCT_DESC_ARG4(structname, __VA_ARGS__)
#define E_DEFINE_STRUCT_DESC_ARG6(structname, param, ...) E_DEFINE_STRUCT_DESC_VAR(structname, param),\
                                                            E_DEFINE_STRUCT_DESC_ARG5(structname, __VA_ARGS__)




#define E_START_STRUCT(name) struct name {
#define E_END_STRUCT(name) };

#define E_START_DESCRIPTION(name) static const auto E_CONCATENATE(name, _Desc) = Engine::ETypeDescription(Engine::EType::STRUCT, E_STRINGIFY(name), 0, sizeof(name), {
#define E_END_DESCRIPTION() });


#define E_STRUCTURE(name, ...)      E_START_STRUCT(name)\
                                    E_CONCATENATE(E_DEFINE_STRUCT_ARG, E_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)\
                                    E_END_STRUCT()\
                                    E_START_DESCRIPTION(name)\
                                    E_CONCATENATE(E_DEFINE_STRUCT_DESC_ARG, E_ARG_COUNT(__VA_ARGS__))(name, __VA_ARGS__)\
                                    E_END_DESCRIPTION()

#define E_STRUCT_DATA(name, type) currently_setting.Childs.push_back({{#name, type}});


#define E_DESC(type) E_CONCATENATE(type, _Desc)
        