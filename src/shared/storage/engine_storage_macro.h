#pragma once



#define E_CREATE_STRUCT_PROP(prop) 



#define E_STORAGE_STRUCT(name, ...) struct name {\
                                        E_LOOP_ARGS(E_CREATE_STRUCT_PROP, __VA_ARGS__)




#define E_STORAGE_TYPE(name, ...) E_STORAGE_STRUCT(name, __VA_ARGS__)