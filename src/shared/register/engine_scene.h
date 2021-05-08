#pragma once

namespace Engine {

    enum class EComponentType
    {
        INTEGER,
        DOUBLE,
        BOOL,
        STRING,

        COMPONENT_REFERENCE,
    };

    struct EComponentTypeDescription
    {
        EComponentType  Type;
        EString         Name;
    };
    typedef EVector<EComponentTypeDescription> TComponentTypeList;

    struct EComponentDescription
    {
        using ComponentID = EString;

        TComponentTypeList  TypeDesciptions;
        ComponentID         ID;

        EComponentDescription(const ComponentID& id, std::initializer_list<EComponentTypeDescription>&& types)
            : TypeDesciptions(types)
        {}
    private:
        friend class EScene;
    };

    class E_API EScene
    {
    public:
        using Entity = u32;
    private:
        EUnorderedMap<EComponentDescription::ComponentID, EComponentDescription> fRegisteredComponents;

        class EComponentStorage
        {
        private:
            EComponentDescription fDsc;

        };

        EUnorderedMap<Entity, EUnorderedMap<EComponentDescription::ComponentID, EComponentStorage>> fComponentStorage;
    public:

        /**
         * Register a new component.
         * These can be instantiated with an object handle to the data
         * @param description The component description
         */
        void RegisterComponent(EComponentDescription description);



    };

}