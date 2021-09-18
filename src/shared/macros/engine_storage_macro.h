#pragma once



#define E_CREATE_STRUCT_PROP1(type, name, ...) type name; 
#define E_CREATE_STRUCT_PROP(nametype) EXPAND( EXPAND ( E_CREATE_STRUCT_PROP1 nametype ) )



#define E_CREATE_STRUCT_DSC2(type, name, ...) {E_STRINGIFY(name), ::Engine::getdsc::GetDescription<type>()},
#define E_CREATE_STRUCT_DSC(nametype) EXPAND( E_CREATE_STRUCT_DSC2 nametype )

#define E_GET_FROM_PROP2(type, name, ...) ::Engine::EProperty* EXPAND ( E_CONCATENATE(prop, name) ) = property->GetProperty(E_STRINGIFY(name));
#define E_GET_FROM_PROP(nametype) EXPAND( E_GET_FROM_PROP2 nametype )

#define E_GET_FROM_PROP_CONST2(type, name, ...) const ::Engine::EProperty* EXPAND ( E_CONCATENATE(prop, name) ) = property->GetProperty(E_STRINGIFY(name));
#define E_GET_FROM_PROP_CONST(nametype) EXPAND( E_GET_FROM_PROP_CONST2 nametype )

#define E_CHECK_NULL_AND2(type, name, ...) EXPAND ( E_CONCATENATE(prop, name) ) &&
#define E_CHECK_NULL_AND(typename) EXPAND ( E_CHECK_NULL_AND2 typename )
#define E_CHECK_NULL_AND_LAST2(type, name, ...) EXPAND ( E_CONCATENATE(prop, name) )
#define E_CHECK_NULL_AND_LAST(typename) EXPAND ( E_CHECK_NULL_AND_LAST2 typename )

#define E_DEFAULT_CONSTRUCTUR2(type, name, ...) name = type(__VA_ARGS__);
#define E_DEFAULT_CONSTRUCTUR(typename) EXPAND ( E_DEFAULT_CONSTRUCTUR2 typename )

#define E_COMPLETE_CONSTRUCTOR2(type, name, ...) name = EXPAND ( E_CONCATENATE(_, name) );
#define E_COMPLETE_CONSTRUCTOR(typename) EXPAND ( E_COMPLETE_CONSTRUCTOR2 typename )

#define E_CONSTRUCTOR_ARG2(type, name, ...) const type& EXPAND ( E_CONCATENATE(_, name) ),
#define E_CONSTRUCTOR_ARG(typename) EXPAND ( E_CONSTRUCTOR_ARG2 typename )

#define E_CONSTRUCTOR_ARG_L2(type, name, ...) const type& EXPAND ( E_CONCATENATE(_, name) )
#define E_CONSTRUCTOR_ARG_LAST(typename) EXPAND ( E_CONSTRUCTOR_ARG_L2 typename )


#define E_SET_PROPERTY2(type, s_name, ...) {::Engine::EValueDescription valDsc = ::Engine::getdsc::GetDescription<type>();\
                                    if constexpr (is_vector<type>::value)\
                                    {\
                                        if (valDsc.GetType() == ::Engine::EValueType::ARRAY)\
                                        {\
                                            static_cast<::Engine::EArrayProperty*>(EXPAND ( E_CONCATENATE(prop, s_name) ) )->SetValue<type>(value. s_name);\
                                        }\
                                        else \
                                        {\
                                            E_ASSERT_M(false, EString("Type missmatch! ") + typeid(type).name() + " is a vector, but the description is not an array!");\
                                            return false;\
                                        }\
                                    }\
                                    else\
                                    {\
                                        switch (valDsc.GetType())\
                                        {\
                                        case ::Engine::EValueType::STRUCT: static_cast<::Engine::EStructProperty*>(EXPAND ( E_CONCATENATE(prop, s_name) ) )->SetValue<type>(value. s_name ); break;\
                                        case ::Engine::EValueType::PRIMITIVE: static_cast<::Engine::EValueProperty<type>*>(EXPAND ( E_CONCATENATE(prop, s_name) ) )->SetValue(value. s_name ); break;\
                                        case ::Engine::EValueType::ENUM: break;/*TODO*/\
                                        case ::Engine::EValueType::UNKNOWN: break;\
                                        }\
                                    }\
                                    }
#define E_SET_PROPERTY(typename) EXPAND ( E_SET_PROPERTY2 typename )


#define E_SET_SELF2(type, s_name, ...) {::Engine::EValueDescription valDsc = ::Engine::getdsc::GetDescription<type>();\
                                    if constexpr (is_vector<type>::value)\
                                    {\
                                        if (valDsc.GetType() == ::Engine::EValueType::ARRAY)\
                                        {\
                                            static_cast<const ::Engine::EArrayProperty*>(EXPAND ( E_CONCATENATE(prop, s_name) ) )->GetValue<type>(value. s_name);\
                                        }\
                                        else\
                                        {\
                                            E_ASSERT_M(false, EString("Type missmatch! ") + typeid(type).name() + " is a vector, but the description is not an array!");\
                                            return false;\
                                        }\
                                    }\
                                    else\
                                    {\
                                        switch (valDsc.GetType())\
                                        {\
                                        case ::Engine::EValueType::STRUCT: static_cast<const ::Engine::EStructProperty*>(EXPAND ( E_CONCATENATE(prop, s_name) ) )->GetValue<type>(value. s_name ); break;\
                                        case ::Engine::EValueType::PRIMITIVE: value. s_name = static_cast<const ::Engine::EValueProperty<type>*>(EXPAND ( E_CONCATENATE(prop, s_name) ) )->GetValue(); break;\
                                        case ::Engine::EValueType::ENUM: break;/*TODO*/\
                                        case ::Engine::EValueType::UNKNOWN: break;\
                                        }\
                                    }\
                                    }
#define E_SET_SELF(typename) EXPAND ( E_SET_SELF2 typename )


#define E_CHECK_EQUEL2(type, name, ...) name == other. name &&
#define E_CHECK_EQUEL(typename) EXPAND (E_CHECK_EQUEL2 typename )

#define E_CHECK_EQUEL_LAST2(type, name, ...) name == other. name
#define E_CHECK_EQUEL_LAST(typename) EXPAND (E_CHECK_EQUEL_LAST2 typename )


#define E_STORAGE_STRUCT(name, ...) struct name {\
                                        EXPAND (E_LOOP_ARGS(E_CREATE_STRUCT_PROP, __VA_ARGS__) )\
                                        static inline ::Engine::EValueDescription _dsc = ::Engine::EValueDescription::CreateStruct(EXPAND(E_STRINGIFY(name)), {\
                                            EXPAND (E_LOOP_ARGS(E_CREATE_STRUCT_DSC, __VA_ARGS__))\
                                        });\
                                        \
                                        name()\
                                        {\
                                            EXPAND ( E_LOOP_ARGS ( E_DEFAULT_CONSTRUCTUR, __VA_ARGS__ ) )\
                                        }\
                                        name ( EXPAND ( E_LOOP_ARGS_L( E_CONSTRUCTOR_ARG, __VA_ARGS__ ) ) )\
                                        {\
                                            EXPAND ( E_LOOP_ARGS ( E_COMPLETE_CONSTRUCTOR, __VA_ARGS__ ) )\
                                        }\
                                        static bool ToProperty(const name & value, ::Engine::EStructProperty* property)\
                                        {\
                                            EXPAND( E_LOOP_ARGS(E_GET_FROM_PROP, __VA_ARGS__) ) \
                                            if (\
                                                EXPAND(E_LOOP_ARGS_L(E_CHECK_NULL_AND, __VA_ARGS__))\
                                            ){\
                                                EXPAND(E_LOOP_ARGS(E_SET_PROPERTY, __VA_ARGS__))\
                                                return true;\
                                            }\
                                            return false;\
                                        }\
                                        static bool FromProperty(name & value, const ::Engine::EStructProperty* property)\
                                        {\
                                            EXPAND( E_LOOP_ARGS(E_GET_FROM_PROP_CONST, __VA_ARGS__) ) \
                                            if (\
                                                EXPAND(E_LOOP_ARGS_L(E_CHECK_NULL_AND, __VA_ARGS__))\
                                            ){\
                                                EXPAND(E_LOOP_ARGS(E_SET_SELF, __VA_ARGS__))\
                                                return true;\
                                            }\
                                            return false;\
                                        }\
                                        bool operator==(const name& other) const {\
                                            return \
                                            EXPAND(E_LOOP_ARGS_L(E_CHECK_EQUEL, __VA_ARGS__));\
                                        }\
                                        bool operator!=(const name& other) const { return !((*this) == other);}\
                                    };


