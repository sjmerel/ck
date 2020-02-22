#import "AppDelegate.h"
#import "ck/ck.h"
#import "ck/config.h"
#import "ck/bank.h"
#import "ck/sound.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)playSound
{
    CkBank* bank = CkBank::newBank("hellocricket.ckb");
    CkSound* sound = CkSound::newBankSound(bank, 0);
    sound->play();

    while (sound->isPlaying())
    {
        CkUpdate();
        usleep(30000);
    }

    sound->destroy();
    bank->destroy();
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application

    CkConfig config;
    CkInit(&config);

    [self playSound];
    CkShutdown();

}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
