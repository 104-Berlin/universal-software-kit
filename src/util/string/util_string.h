#pragma once

namespace Engine {

    namespace EStringUtil {

        EVector<EString> SplitString(const EString& value, const EString& delim);
        EString ToLower(const EString& value);

    }

}