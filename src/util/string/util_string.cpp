#include "prefix_util.h"

using namespace Engine;

EVector<EString> EStringUtil::SplitString(const EString& value, const EString& delim) 
{
    EVector<EString> result;
    if (value.empty()) { return result; }
    size_t last_pos = 0;
    size_t pos = last_pos;
    EString token;
    while ((pos = value.find(delim, last_pos)) != EString::npos) {
        result.push_back(value.substr(last_pos, pos - last_pos));

        last_pos = pos + delim.length();
    }
    if (last_pos < value.length())
    {
        result.push_back(value.substr(last_pos, value.length() - last_pos));
    }
    return result;
}

EString EStringUtil::ToLower(const EString& value) 
{
    EString result = value;
    std::transform(result.begin(), result.end(), result.begin(),
                    [](unsigned char c){ return std::tolower(c); });
    return result;
}
