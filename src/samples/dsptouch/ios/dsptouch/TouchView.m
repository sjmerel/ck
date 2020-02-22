#import "TouchView.h"
#import "dsptouch.h"

@interface TouchView ()

@end

@implementation TouchView

- (void)setParams:(NSSet *)touches
{
    UITouch* touch = [touches anyObject];
    CGPoint p = [touch locationInView:self];
    CGSize size = self.frame.size;
    dspTouchSetEffectParams(p.x/size.width, (size.height-p.y)/size.height);
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    if (self = [super initWithCoder:aDecoder])
    {
        m_timer = [NSTimer scheduledTimerWithTimeInterval: 0.030 target:self selector:@selector(onTimer:) userInfo:nil repeats: YES];
    }
    return self;
}

- (void)dealloc
{
    [m_timer invalidate];
}

- (void) onTimer:(NSTimer*)timer
{
    dspTouchUpdate();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self setParams:touches];
    dspTouchStartEffect();
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    dspTouchStopEffect();
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self setParams:touches];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    dspTouchStopEffect();
}

@end

