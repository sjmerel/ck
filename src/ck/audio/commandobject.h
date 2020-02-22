#pragma once

#include "ck/core/platform.h"
#include "ck/audio/commandparam.h"

namespace Cki
{


class CommandObject
{
public:
    virtual ~CommandObject() {}

    // This should be called ONLY by AudioGraph,
    // so that it is called from the audio thread.
    virtual void execute(int commandId, CommandParam param0, CommandParam param1);
};


}
