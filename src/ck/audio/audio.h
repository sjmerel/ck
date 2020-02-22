#pragma once

#include "ck/core/platform.h"

namespace Cki
{


namespace Audio
{
    bool init();
    void shutdown();
    void update();
    void suspend();
    void resume();
}


}
