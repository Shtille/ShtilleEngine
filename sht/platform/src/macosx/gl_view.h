#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>

@interface ShtilleEngineGLView : NSOpenGLView {
	CVDisplayLinkRef displayLink;
}

@end
