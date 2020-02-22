#include "ck/core/target.h"
#include "ck/core/types.h"

namespace Cki
{


namespace Target
{
    const char* getName(Value value)
    {
        switch (value)
        {
            case k_ios:     return "ios";
            case k_android: return "android";
            case k_osx:     return "osx";
            case k_win:     return "win";
            case k_wp8:     return "wp8";
            case k_linux:   return "linux";
            case k_tvos:    return "tvos";
            default:        return NULL;
        }
    }
}


}
