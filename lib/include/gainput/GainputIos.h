
#ifndef GAINPUTIOS_H_
#define GAINPUTIOS_H_

#import <UIKit/UIKit.h>

namespace gainput
{
    class InputManager;
}

@interface GainputView : UIView

- (id)initWithFrame:(CGRect)frame inputManager:(gainput::InputManager&)inputManager;

@end

#endif

