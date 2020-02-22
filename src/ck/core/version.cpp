#include "ck/core/version.h"

namespace Cki
{

namespace Version
{

const int k_major =
#include "../../../etc/version/major.txt"
    ;

const int k_minor = 
#include "../../../etc/version/minor.txt"
    ;

const int k_build =
#include "../../../etc/version/build.txt"
    ;

const char* k_label = 
#include "../../../etc/version/label.txt"
    ;

}

}

