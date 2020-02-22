#import "AppDelegate.h"
#import "ck/objc/ck.h"
#import "ck/objc/config.h"
#import "ck/objc/bank.h"
#import "ck/objc/sound.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)playSound
{
    CkoBank* bank = [CkoBank newBank:@"hellocricket.ckb"];
    CkoSound* sound = [CkoSound newBankSound:bank index:0];
    [sound play];

    while (sound.playing)
    {
        CkUpdate();
        usleep(30000);
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    CkConfig config;
    CkConfigInit(&config);
    CkInit(&config);

    [self playSound];

    CkShutdown();
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
