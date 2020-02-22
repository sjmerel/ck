#import <UIKit/UIKit.h>
#import "game.h"

int main(int argc, char *argv[])
{
    @autoreleasepool
    {
        printf("game init\n");
        gameInit();

        int retVal = UIApplicationMain(argc, argv, nil, nil);

        printf("game shutdown\n");
        gameShutdown();

        return retVal;
    }
}
