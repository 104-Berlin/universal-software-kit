#pragma once



#define E_CREATE_STRUCT_PROP1(name, type) type name; 
#define E_CREATE_STRUCT_PROP(nametype) E_CREATE_STRUCT_PROP1 typename 

#define E_CREATE_STRUCT_DSC2(name, type) {E_STRINGIFY(name), getdsc::GetDescription<type>()},
#define E_CREATE_STRUCT_DSC(nametype) E_CREATE_STRUCT_DSC2 nametype

/*
struct name {
    int SomeValue;

    static EValueDescription _dsc = EValueDescription::CreateStruct("name", {
        {"SomeValue", getdsc::GetDescription<int>()}
    });
};
*/


#define E_STORAGE_STRUCT(name, ...) struct name {\
                                        E_LOOP_ARGS(E_CREATE_STRUCT_PROP, __VA_ARGS__)\
                                        static EValueDescription _dsc = EValueDescription::CreateStruct(E_STRINGIFY(name), {\
                                            E_LOOP_ARGS(E_CREATE_STRUCT_DSC, __VA_ARGS__)\
                                        });\
                                    };




#define E_STORAGE_TYPE(name, ...) E_STORAGE_STRUCT(name, __VA_ARGS__)