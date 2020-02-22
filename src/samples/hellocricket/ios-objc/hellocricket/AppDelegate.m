#import "AppDelegate.h"
#import "ck/objc/ck.h"
#import "ck/objc/config.h"
#import "ck/objc/bank.h"
#import "ck/objc/sound.h"

@interface AppDelegate ()

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

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.

    CkConfig config;
    CkConfigInit(&config);
    CkInit(&config);

    [self playSound];

    CkShutdown();

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
