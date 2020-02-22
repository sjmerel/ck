#import "ck/api/objc/cko.h"
#import "ck/api/objc/proxy.h"

void CkoInit()
{
    CkoProxyInit();
}

void CkoShutdown()
{
    CkoProxyShutdown();
}
