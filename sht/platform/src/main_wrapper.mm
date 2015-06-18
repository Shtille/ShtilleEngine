#include "../include/main_wrapper.h"
#include "../../common/platform.h"

#if defined(TARGET_MAC)
#import <Cocoa/Cocoa.h>
#elif defined(TARGET_IOS)
#import <UIKit/UIKit.h>
#else
#error Wrong platform, check included source files
#endif

int MainWrapper(int argc, const char** argv)
{
#if defined(TARGET_MAC)
	return NSApplicationMain(argc, argv);
#elif defined(TARGET_IOS)
	@autoreleasepool {
        return UIApplicationMain(argc, const_cast<char**>(argv), nil, NSStringFromClass([AppDelegate class]));
    }
#endif
}