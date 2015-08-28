#import <Cocoa/Cocoa.h>
#import "gl_view.h"

@interface ShtilleEngineWindowController : NSWindowController {

	// IBOutlet must be used so that, in Inteface Builder,
	// we can connect the view in the NIB to windowedView
	IBOutlet ShtilleEngineGLView *view;
}

@end
