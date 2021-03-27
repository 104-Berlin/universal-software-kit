#pragma once

namespace Engine {

    /**
     * The basic Descriptor for all Registratable things
     */
    class E_API EDescriptor
    {
    private:
        EString fName;
    public:
        EDescriptor(const EString& name);
    };

}