#import <UIKit/UIKit.h>

@class gameViewController;

@interface gameAppDelegate : NSObject <UIApplicationDelegate> {

}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@property (nonatomic, retain) IBOutlet gameViewController *viewController;

@end
