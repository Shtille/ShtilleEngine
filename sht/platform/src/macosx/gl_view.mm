#import "gl_view.h"

#include "../../../application/application.h"
#include "../../../system/include/keys.h"

#define SUPPORT_RETINA_RESOLUTION 1
#define ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 1

// Translates OS X key modifiers to engine ones
static int TranslateModifiers(NSUInteger mods)
{
    int modifier = 0;
    if (mods & NSShiftKeyMask)
        modifier |= sht::ModifierKey::kShift;
    if (mods & NSControlKeyMask)
        modifier |= sht::ModifierKey::kControl;
    if (mods & NSAlternateKeyMask)
        modifier |= sht::ModifierKey::kAlt;
    if (mods & NSCommandKeyMask)
        modifier |= sht::ModifierKey::kSuper;
    return modifier;
}

// Translates OS X mouse button numbers to engine ones
static sht::MouseButton TranslateMouseButton(int button)
{
    sht::MouseButton translated = sht::MouseButton::kUnknown;
    switch (button)
    {
        case 2:
            translated = sht::MouseButton::kMiddle;
            break;
    }
    return translated;
}

@interface ShtilleEngineGLView (PrivateMethods)
- (void) initGL;

@end

@implementation ShtilleEngineGLView

- (CVReturn) getFrameForTime:(const CVTimeStamp*)outputTime
{
	// There is no autorelease pool when this method is called
	// because it will be called from a background thread.
	// It's important to create one or app can leak objects.
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    double deltaTime = 1.0 / (outputTime->rateScalar * (double)outputTime->videoTimeScale / (double)outputTime->videoRefreshPeriod);
	
    [self drawView:deltaTime];
	
	[pool release];
	return kCVReturnSuccess;
}

// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
									  const CVTimeStamp* now,
									  const CVTimeStamp* outputTime,
									  CVOptionFlags flagsIn,
									  CVOptionFlags* flagsOut, 
									  void* displayLinkContext)
{
    CVReturn result = [(ShtilleEngineGLView*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (void) awakeFromNib
{
    NSOpenGLPixelFormatAttribute attrs[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		// Must specify the 3.2 Core Profile to use OpenGL 3.2
#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 
		NSOpenGLPFAOpenGLProfile,
		NSOpenGLProfileVersion3_2Core,
#endif
		0
	};
	
	NSOpenGLPixelFormat *pf = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs] autorelease];
	
	if (!pf)
	{
		NSLog(@"No OpenGL pixel format");
	}
	   
    NSOpenGLContext* context = [[[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil] autorelease];
    
#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 && defined(DEBUG)
	// When we're using a CoreProfile context, crash if we call a legacy OpenGL function
	// This will make it much more obvious where and when such a function call is made so
	// that we can remove such calls.
	// Without this we'd simply get GL_INVALID_OPERATION error for calling legacy functions
	// but it would be more difficult to see where that function was called.
	CGLEnable([context CGLContextObj], kCGLCECrashOnRemovedFunctions);
#endif
	
    [self setPixelFormat:pf];
    
    [self setOpenGLContext:context];
    
#if SUPPORT_RETINA_RESOLUTION
    // Opt-In to Retina resolution
    [self setWantsBestResolutionOpenGLSurface:YES];
#endif // SUPPORT_RETINA_RESOLUTION
}

- (void) prepareOpenGL
{
	[super prepareOpenGL];
	
	// Make all the OpenGL calls to setup rendering  
	//  and build the necessary rendering objects
	[self initGL];
	
	// Create a display link capable of being used with all active displays
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	
	// Set the renderer output callback function
	CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
	
	// Set the display link for the current renderer
	CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
	CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
	
	// Activate the display link
	CVDisplayLinkStart(displayLink);
	
	// Register to be notified when the window closes so we can stop the displaylink
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(windowWillClose:)
												 name:NSWindowWillCloseNotification
											   object:[self window]];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(windowDidMiniaturize:)
                                                 name:NSWindowDidMiniaturizeNotification
                                               object:[self window]];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(windowDidDeminiaturize:)
                                                 name:NSWindowDidDeminiaturizeNotification
                                               object:[self window]];
}

- (void) windowWillClose:(NSNotification*)notification
{
	// Stop the display link when the window is closing because default
	// OpenGL render buffers will be destroyed.  If display link continues to
	// fire without renderbuffers, OpenGL draw calls will set errors.
	
	CVDisplayLinkStop(displayLink);
}

- (void) windowDidMiniaturize:(NSNotification*)notification
{
    sht::Application * app = sht::Application::GetInstance();
    app->set_visible(false);
}

- (void) windowDidDeminiaturize:(NSNotification*)notification
{
    sht::Application * app = sht::Application::GetInstance();
    app->set_visible(true);
}

- (void) initGL
{
	// The reshape function may have changed the thread to which our OpenGL
	// context is attached before prepareOpenGL and initGL are called.  So call
	// makeCurrentContext to ensure that our OpenGL context current to this 
	// thread (i.e. makeCurrentContext directs all OpenGL calls on this thread
	// to [self openGLContext])
	[[self openGLContext] makeCurrentContext];
	
	// Synchronize buffer swaps with vertical refresh rate
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	
	// Init our renderer.  Use 0 for the defaultFBO which is appropriate for
	// OSX (but not iOS since iOS apps must create their own FBO)
    //[self doInitWindowSize];
    
    sht::Application * app = sht::Application::GetInstance();
    if (app->InitApi()) // context is already created, but we also need a renderer
    {
        if (app->Load()) // try to load user data
        {
            // everything is OK
        }
        else
        {
            NSLog(@"User data loading failed");
            abort();
        }
    }
    else
    {
        NSLog(@"Context initialization failed");
        abort();
    }
}

- (void) reshape
{
	[super reshape];
	
	// We draw on a secondary thread through the display link. However, when
	// resizing the view, -drawRect is called on the main thread.
	// Add a mutex around to avoid the threads accessing the context
	// simultaneously when resizing.
	CGLLockContext([[self openGLContext] CGLContextObj]);

	// Get the view size in Points
	NSRect viewRectPoints = [self bounds];
    
#if SUPPORT_RETINA_RESOLUTION

    // Rendering at retina resolutions will reduce aliasing, but at the potential
    // cost of framerate and battery life due to the GPU needing to render more
    // pixels.

    // Any calculations the renderer does which use pixel dimentions, must be
    // in "retina" space.  [NSView convertRectToBacking] converts point sizes
    // to pixel sizes.  Thus the renderer gets the size in pixels, not points,
    // so that it can set it's viewport and perform and other pixel based
    // calculations appropriately.
    // viewRectPixels will be larger (2x) than viewRectPoints for retina displays.
    // viewRectPixels will be the same as viewRectPoints for non-retina displays
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
#else //if !SUPPORT_RETINA_RESOLUTION
    
    // App will typically render faster and use less power rendering at
    // non-retina resolutions since the GPU needs to render less pixels.  There
    // is the cost of more aliasing, but it will be no-worse than on a Mac
    // without a retina display.
    
    // Points:Pixels is always 1:1 when not supporting retina resolutions
    NSRect viewRectPixels = viewRectPoints;
    
#endif // !SUPPORT_RETINA_RESOLUTION
    
	// Set the new dimensions in our renderer
    if ([self.window isVisible]) // make sure context is created
    {
        sht::Application * app = sht::Application::GetInstance();
        app->OnSize(viewRectPixels.size.width, viewRectPixels.size.height);
    }
	
	CGLUnlockContext([[self openGLContext] CGLContextObj]);
}

- (void)viewDidUnhide
{
}

- (void)renewGState
{	
	// Called whenever graphics state updated (such as window resize)
	
	// OpenGL rendering is not synchronous with other rendering on the OSX.
	// Therefore, call disableScreenUpdatesUntilFlush so the window server
	// doesn't render non-OpenGL content in the window asynchronously from
	// OpenGL content, which could cause flickering.  (non-OpenGL content
	// includes the title bar and drawing done by the app with other APIs)
	[[self window] disableScreenUpdatesUntilFlush];

	[super renewGState];
}

- (void) drawRect: (NSRect) theRect
{
	// Called during resize operations
	
	// Avoid flickering during resize by drawiing	
    [self drawView:0.0];
}

- (void) drawView: (double) deltaTime
{	 
	[[self openGLContext] makeCurrentContext];

	// We draw on a secondary thread through the display link
	// When resizing the view, -reshape is called automatically on the main
	// thread. Add a mutex around to avoid the threads accessing the context
	// simultaneously when resizing
	CGLLockContext([[self openGLContext] CGLContextObj]);

    sht::Application * app = sht::Application::GetInstance();
    if (app->visible()) // make sure context is created
    {
        app->SetFrameTime((float)deltaTime);
        app->Update();
        app->Render();
    }

	CGLFlushDrawable([[self openGLContext] CGLContextObj]);
	CGLUnlockContext([[self openGLContext] CGLContextObj]);
}

- (void) dealloc
{
	// Stop the display link BEFORE releasing anything in the view
    // otherwise the display link thread may call into the view and crash
    // when it encounters something that has been release
	CVDisplayLinkStop(displayLink);

	CVDisplayLinkRelease(displayLink);

	// Release the display link AFTER display link has been released
    sht::Application * app = sht::Application::GetInstance();
    app->Unload();
    app->DeinitApi();
	
	[super dealloc];
}

- (BOOL) isOpaque
{
    return YES;
}
- (BOOL) canBecomeKeyView
{
    return YES;
}
- (BOOL) acceptsFirstResponder
{
    return YES;
}

// ----- Keyboard events -----

- (void) keyDown:(NSEvent *)theEvent
{
    const unsigned short key_code = [theEvent keyCode];
    const int modifiers = TranslateModifiers([theEvent modifierFlags]);
    
    sht::Application * app = sht::Application::GetInstance();
    sht::PublicKey translated_key = app->keys().table(key_code);
    app->keys().key_down(translated_key) = true;
    app->keys().modifiers() = modifiers;
    
    app->OnKeyDown(translated_key, modifiers);
}

- (void) keyUp:(NSEvent *)theEvent
{
    const unsigned short key_code = [theEvent keyCode];
    const int modifiers = TranslateModifiers([theEvent modifierFlags]);
    
    sht::Application * app = sht::Application::GetInstance();
    sht::PublicKey translated_key = app->keys().table(key_code);
    app->keys().key_down(translated_key) = false;
    app->keys().modifiers() = 0;
    
    app->OnKeyUp(translated_key, modifiers);
}

- (void) flagsChanged:(NSEvent *)theEvent
{
    const unsigned short key_code = [theEvent keyCode];
    const int modifiers = TranslateModifiers([theEvent modifierFlags] & NSDeviceIndependentModifierFlagsMask);
    
    sht::Application * app = sht::Application::GetInstance();
    sht::PublicKey translated_key = app->keys().table(key_code);
    
    bool press;
    if (app->keys().modifiers() == modifiers)
    {
        press = ! app->keys().key_down(translated_key);
        app->keys().key_down(translated_key) = press;
    }
    else
    {
        press = modifiers > app->keys().modifiers();
        app->keys().modifiers() = modifiers;
    }
    if (press)
        app->OnKeyDown(translated_key, modifiers);
    else
        app->OnKeyUp(translated_key, modifiers);
}

// ----- Mouse events -----

- (void) mouseDown:(NSEvent *)theEvent
{
    sht::Application * app = sht::Application::GetInstance();
    app->mouse().button_down(sht::MouseButton::kLeft) = true;
    app->OnMouseDown(sht::MouseButton::kLeft, TranslateModifiers([theEvent modifierFlags]));
}

- (void) mouseUp:(NSEvent *)theEvent
{
    sht::Application * app = sht::Application::GetInstance();
    app->mouse().button_down(sht::MouseButton::kLeft) = false;
    app->OnMouseUp(sht::MouseButton::kLeft, TranslateModifiers([theEvent modifierFlags]));
}

- (void) mouseMoved:(NSEvent *)theEvent
{
    sht::Application * app = sht::Application::GetInstance();
    app->mouse().delta_x() = [theEvent deltaX];
    app->mouse().delta_y() = [theEvent deltaY];
    NSPoint pos = [theEvent locationInWindow];
    app->mouse().x() = pos.x;
    app->mouse().y() = pos.y;
    app->OnMouseMove();
}

- (void) mouseDragged:(NSEvent *)theEvent
{
    [self mouseMoved:theEvent];
}

- (void) rightMouseDown:(NSEvent *)theEvent
{
    sht::Application * app = sht::Application::GetInstance();
    app->mouse().button_down(sht::MouseButton::kRight) = true;
    app->OnMouseDown(sht::MouseButton::kRight, TranslateModifiers([theEvent modifierFlags]));
}

- (void) rightMouseUp:(NSEvent *)theEvent
{
    sht::Application * app = sht::Application::GetInstance();
    app->mouse().button_down(sht::MouseButton::kRight) = false;
    app->OnMouseUp(sht::MouseButton::kRight, TranslateModifiers([theEvent modifierFlags]));
}

- (void) rightMouseDragged:(NSEvent *)theEvent
{
    [self mouseMoved:theEvent];
}

- (void) otherMouseDown:(NSEvent *)theEvent
{
    sht::Application * app = sht::Application::GetInstance();
    sht::MouseButton button = TranslateMouseButton((int)[theEvent buttonNumber]);
    app->mouse().button_down(button) = true;
    app->OnMouseDown(button, TranslateModifiers([theEvent modifierFlags]));
}

- (void) otherMouseUp:(NSEvent *)theEvent
{
    sht::Application * app = sht::Application::GetInstance();
    sht::MouseButton button = TranslateMouseButton((int)[theEvent buttonNumber]);
    app->mouse().button_down(button) = false;
    app->OnMouseUp(button, TranslateModifiers([theEvent modifierFlags]));
}

- (void) otherMouseDragged:(NSEvent *)theEvent
{
    [self mouseMoved:theEvent];
}

- (void) scrollWheel:(NSEvent *)theEvent
{
    float delta_x = [theEvent scrollingDeltaX];
    float delta_y = [theEvent scrollingDeltaY];
    if ([theEvent hasPreciseScrollingDeltas])
    {
        delta_x *= 0.1f;
        delta_y *= 0.1f;
    }
    sht::Application * app = sht::Application::GetInstance();
    app->OnScroll(delta_x, delta_y);
}

@end
