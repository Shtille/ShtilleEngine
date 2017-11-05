#import "fullscreen_window.h"

#include "../../include/window_controller.h"
#include "../platform_inner.h"

#include "../window_struct.h"

#include "../../../application/application.h"

#include <vector> // for attributes
#include <stdio.h> // for error logging
#include <crt_externs.h> // for _NSGetProgname
#include <Availability.h>

static PlatformWindow g_window;

static void LogError(const char* message)
{
    fprintf(stderr, "%s\n", message);
}

//------------------------------------------------------------------------
// Delegate for window related notifications
//------------------------------------------------------------------------

@interface ShtilleEngineWindowDelegate : NSObject
@end

@implementation ShtilleEngineWindowDelegate

- (BOOL)windowShouldClose:(id)sender
{
    g_window.need_quit = true;
    return NO;
}

- (void)windowDidResize:(NSNotification *)notification
{
    const NSRect viewRectPoints = [g_window.view frame];
    const NSRect viewRectPixels = [g_window.view convertRectToBacking:viewRectPoints];

    sht::Application * app = sht::Application::GetInstance();
    app->OnSize(viewRectPixels.size.width, viewRectPixels.size.height);
}

- (void)windowDidMove:(NSNotification *)notification
{
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
    sht::Application * app = sht::Application::GetInstance();
    app->set_visible(false);
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
    sht::Application * app = sht::Application::GetInstance();
    app->set_visible(true);
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
    // if (window->monitor)
    //     enterFullscreenMode(window);
}

- (void)windowDidResignKey:(NSNotification *)notification
{
    // if (window->monitor)
    //     leaveFullscreenMode(window);
}

- (NSSize)window:(NSWindow *)window willUseFullScreenContentSize:(NSSize)proposedSize
{
    NSRect screenRect = [[NSScreen mainScreen] frame];
    return screenRect.size;
}

@end

//------------------------------------------------------------------------
// Delegate for application related notifications
//------------------------------------------------------------------------

@interface ShtilleEngineApplicationDelegate : NSObject
@end

@implementation ShtilleEngineApplicationDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    g_window.need_quit = true;
    return NSTerminateCancel;
}

- (void)applicationDidChangeScreenParameters:(NSNotification *) notification
{
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    [NSApp stop:nil];

    // Post empty event
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    NSEvent* event = [NSEvent 
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
                              otherEventWithType:NSApplicationDefined
#else
                              otherEventWithType:NSEventTypeApplicationDefined
#endif
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
    [NSApp postEvent:event atStart:YES];
    [pool drain];
}

@end

// Translates OS X key modifiers to engine ones
static int TranslateModifiers(NSUInteger mods)
{
    int modifier = 0;
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
    if (mods & NSShiftKeyMask)
        modifier |= sht::ModifierKey::kShift;
    if (mods & NSControlKeyMask)
        modifier |= sht::ModifierKey::kControl;
    if (mods & NSAlternateKeyMask)
        modifier |= sht::ModifierKey::kAlt;
    if (mods & NSCommandKeyMask)
        modifier |= sht::ModifierKey::kSuper;
#else
    if (mods & NSEventModifierFlagShift)
        modifier |= sht::ModifierKey::kShift;
    if (mods & NSEventModifierFlagControl)
        modifier |= sht::ModifierKey::kControl;
    if (mods & NSEventModifierFlagOption)
        modifier |= sht::ModifierKey::kAlt;
    if (mods & NSEventModifierFlagCommand)
        modifier |= sht::ModifierKey::kSuper;
#endif
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

//------------------------------------------------------------------------
// Content view class for the Shtille Engine window
//------------------------------------------------------------------------

@interface ShtilleEngineContentView : NSOpenGLView

-(id)init;

@end

@implementation ShtilleEngineContentView

-(id)init
{
    self = [super initWithFrame:NSMakeRect(0, 0, 1, 1)
                    pixelFormat:nil];
    return self;
}

-(void)dealloc
{
    [super dealloc];
}

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
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

- (void)viewDidChangeBackingProperties
{
    const NSRect viewRectPoints = [g_window.view frame];
    const NSRect viewRectPixels = [g_window.view convertRectToBacking:viewRectPoints];

    sht::Application * app = sht::Application::GetInstance();
    app->OnSize(viewRectPixels.size.width, viewRectPixels.size.height);
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
    
    NSString* characters = [theEvent characters];
    NSUInteger i, length = [characters length];
    
    for (i = 0; i < length; ++i)
    {
        const unichar codepoint = [characters characterAtIndex:i];
        if ((codepoint & 0xff00) == 0xf700)
            continue;
        
        if (sht::Keys::IsGoodChar(codepoint))
            app->OnChar(codepoint);
    }
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
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
    const int kFlagsMask = NSDeviceIndependentModifierFlagsMask;
#else
    const int kFlagsMask = NSEventModifierFlagDeviceIndependentFlagsMask;
#endif
    const int modifiers = TranslateModifiers([theEvent modifierFlags] & kFlagsMask);
    
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
    NSRect pointRect = NSMakeRect(pos.x, pos.y, 1, 1);
    NSRect pixelRect = [g_window.view convertRectToBacking:pointRect];
    app->mouse().x() = pixelRect.origin.x;
    app->mouse().y() = pixelRect.origin.y;
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

//------------------------------------------------------------------------
// Shtille Engine window class
//------------------------------------------------------------------------

@interface ShtilleEngineWindow : NSWindow
{
    ShtilleEngineFullscreenWindow * fullscreenWindow;
}
- (void)goFullscreen;
- (void)goWindow;
@end

@implementation ShtilleEngineWindow

- (id)init
{
    self = [super init];
    if (self)
        fullscreenWindow = nil;
    return self;
}

- (void)dealloc
{
    if (fullscreenWindow)
    {
        [fullscreenWindow release];
        fullscreenWindow = nil;
    }
    [super dealloc];
}

- (BOOL)canBecomeKeyWindow
{
    // Required for NSBorderlessWindowMask windows
    return YES;
}

- (void)toggleFullScreen:(id)sender
{
    sht::Application * app = sht::Application::GetInstance();
    app->ToggleFullscreen();
}

- (void)goFullscreen
{
    // If app is already fullscreen...
    if(fullscreenWindow)
    {
        //...don't do anything
        return;
    }
    
    // Allocate a new fullscreen window
    fullscreenWindow = [[ShtilleEngineFullscreenWindow alloc] init];
    
    // Resize the view to screensize
    NSRect viewRect = [fullscreenWindow frame];
    
    // Set the view to the size of the fullscreen window
    [g_window.view setFrameSize: viewRect.size];
    
    // Set the view in the fullscreen window
    [fullscreenWindow setContentView:g_window.view];
    
    // Hide non-fullscreen window so it doesn't show up when switching out
    // of this app (i.e. with CMD-TAB)
    [g_window.object orderOut:self];
    
    // Show the window and make it the key window for input
    [fullscreenWindow makeKeyAndOrderFront:self];
    [fullscreenWindow setAcceptsMouseMovedEvents:YES];
}

- (void)goWindow
{
    // If controller doesn't have a full screen window...
    if(fullscreenWindow == nil)
    {
        //...app is already windowed so don't do anything
        return;
    }
    
    // Get the rectangle of the original window
    NSRect viewRect = [g_window.object frame];
    
    // Set the view rect to the new size
    [g_window.view setFrame:viewRect];
    
    // Set the content of the orginal window to the view
    [g_window.object setContentView:g_window.view];
    
    // Show the window and make it the key window for input
    [g_window.object makeKeyAndOrderFront:self];
    
    // Release the fullscreen window
    [fullscreenWindow release];
    
    // Ensure we set fullscreen Window to nil so our checks for
    // windowed vs. fullscreen mode elsewhere are correct
    fullscreenWindow = nil;
}

@end

//------------------------------------------------------------------------
// Shtille Engine application class
//------------------------------------------------------------------------

@interface ShtilleEngineApplication : NSApplication
@end

@implementation ShtilleEngineApplication

// From http://cocoadev.com/index.pl?GameKeyboardHandlingAlmost
// This works around an AppKit bug, where key up events while holding
// down the command key don't get sent to the key window.
- (void)sendEvent:(NSEvent *)event
{
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
    const NSEventType kEventTypeKeyUp = NSKeyUp;
    const NSEventModifierFlags kCommandKeyMask = NSCommandKeyMask;
#else
    const NSEventType kEventTypeKeyUp = NSEventTypeKeyUp;
    const NSEventModifierFlags kCommandKeyMask = NSEventModifierFlagCommand;
#endif
    if ([event type] == kEventTypeKeyUp && ([event modifierFlags] & kCommandKeyMask))
        [[self keyWindow] sendEvent:event];
    else
        [super sendEvent:event];
}

@end

// Try to figure out what the calling application is called
//
static NSString* FindAppName(void)
{
    size_t i;
    NSDictionary* infoDictionary = [[NSBundle mainBundle] infoDictionary];

    // Keys to search for as potential application names
    NSString* GLFWNameKeys[] =
    {
        @"CFBundleDisplayName",
        @"CFBundleName",
        @"CFBundleExecutable",
    };

    for (i = 0;  i < sizeof(GLFWNameKeys) / sizeof(GLFWNameKeys[0]);  i++)
    {
        id name = [infoDictionary objectForKey:GLFWNameKeys[i]];
        if (name &&
            [name isKindOfClass:[NSString class]] &&
            ![name isEqualToString:@""])
        {
            return name;
        }
    }

    char** progname = _NSGetProgname();
    if (progname && *progname)
        return [NSString stringWithUTF8String:*progname];

    // Really shouldn't get here
    return @"Shtille Engine Application";
}

// Set up the menu bar (manually)
// This is nasty, nasty stuff -- calls to undocumented semi-private APIs that
// could go away at any moment, lots of stuff that really should be
// localize(d|able), etc.  Loading a nib would save us this horror, but that
// doesn't seem like a good thing to require of Shtille Engine users.
//
static void CreateMenuBar()
{
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
    const NSEventModifierFlags kAlternateKeyMask = NSAlternateKeyMask;
    const NSEventModifierFlags kCommandKeyMask = NSCommandKeyMask;
    const NSEventModifierFlags kControlKeyMask = NSControlKeyMask;
#else
    const NSEventModifierFlags kAlternateKeyMask = NSEventModifierFlagOption;
    const NSEventModifierFlags kCommandKeyMask = NSEventModifierFlagCommand;
    const NSEventModifierFlags kControlKeyMask = NSEventModifierFlagControl;
#endif

	NSString* appName = FindAppName();

    NSMenu* bar = [[NSMenu alloc] init];
    [NSApp setMainMenu:bar];

    NSMenuItem* appMenuItem =
        [bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
    NSMenu* appMenu = [[NSMenu alloc] init];
    [appMenuItem setSubmenu:appMenu];

    [appMenu addItemWithTitle:[NSString stringWithFormat:@"About %@", appName]
                       action:@selector(orderFrontStandardAboutPanel:)
                keyEquivalent:@""];
    [appMenu addItem:[NSMenuItem separatorItem]];
    NSMenu* servicesMenu = [[NSMenu alloc] init];
    [NSApp setServicesMenu:servicesMenu];
    [[appMenu addItemWithTitle:@"Services"
                       action:NULL
                keyEquivalent:@""] setSubmenu:servicesMenu];
    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItemWithTitle:[NSString stringWithFormat:@"Hide %@", appName]
                       action:@selector(hide:)
                keyEquivalent:@"h"];
    [[appMenu addItemWithTitle:@"Hide Others"
                       action:@selector(hideOtherApplications:)
                keyEquivalent:@"h"]
        setKeyEquivalentModifierMask:kAlternateKeyMask | kCommandKeyMask];
    [appMenu addItemWithTitle:@"Show All"
                       action:@selector(unhideAllApplications:)
                keyEquivalent:@""];
    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", appName]
                       action:@selector(terminate:)
                keyEquivalent:@"q"];

    NSMenuItem* windowMenuItem =
        [bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
    NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    [NSApp setWindowsMenu:windowMenu];
    [windowMenuItem setSubmenu:windowMenu];

    [windowMenu addItemWithTitle:@"Minimize"
                          action:@selector(performMiniaturize:)
                   keyEquivalent:@"m"];
    [windowMenu addItemWithTitle:@"Zoom"
                          action:@selector(performZoom:)
                   keyEquivalent:@""];
    [windowMenu addItem:[NSMenuItem separatorItem]];
    [windowMenu addItemWithTitle:@"Bring All to Front"
                          action:@selector(arrangeInFront:)
                   keyEquivalent:@""];

    [windowMenu addItem:[NSMenuItem separatorItem]];
    [[windowMenu addItemWithTitle:@"Enter Full Screen"
                           action:@selector(toggleFullScreen:)
                    keyEquivalent:@"f"]
        setKeyEquivalentModifierMask:kControlKeyMask | kCommandKeyMask];

    // Prior to Snow Leopard, we need to use this oddly-named semi-private API
    // to get the application menu working properly.
    SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
    [NSApp performSelector:setAppleMenuSelector withObject:appMenu];
}

static bool InitializeAppKit()
{
	if (NSApp)
        return true;

    // Implicitly create shared NSApplication instance
    [ShtilleEngineApplication sharedApplication];

    // In case we are unbundled, make us a proper UI application
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // Menu bar setup must go between sharedApplication above and
    // finishLaunching below, in order to properly emulate the behavior
    // of NSApplicationMain
    CreateMenuBar();

    // There can only be one application delegate, but we allocate it the
    // first time a window is created to keep all window code in this file
    g_window.app_delegate = [[ShtilleEngineApplicationDelegate alloc] init];
    if (g_window.app_delegate == nil)
    {
        LogError("Cocoa: failed to create application delegate");
    	return false;
    }

    [NSApp setDelegate:g_window.app_delegate];
    [NSApp run];

    return true;
}

static bool CreateWindow()
{
    sht::Application * app = sht::Application::GetInstance();

	g_window.delegate = [[ShtilleEngineWindowDelegate alloc] init];
    if (g_window.delegate == nil)
    {
        LogError("Cocoa: Failed to create window delegate");
        return false;
    }

    unsigned int styleMask = 0;

    if (app->IsDecorated())
    {
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
        styleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
#else
        styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
#endif

        if (app->IsResizable())
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
            styleMask |= NSResizableWindowMask;
#else
            styleMask |= NSWindowStyleMaskResizable;
#endif
    }
    else
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
        styleMask = NSBorderlessWindowMask;
#else
        styleMask = NSWindowStyleMaskBorderless;
#endif

    // We will create fullscreen window separately
    NSRect contentRect = NSMakeRect(0, 0, app->width(), app->height());

    g_window.object = [[ShtilleEngineWindow alloc]
        initWithContentRect:contentRect
                  styleMask:styleMask
                    backing:NSBackingStoreBuffered
                      defer:NO];

    if (g_window.object == nil)
    {
        LogError("Cocoa: Failed to create window");
        return false;
    }

    if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
    {
        if (app->IsResizable())
            [g_window.object setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    }

    [g_window.object setTitle:[NSString stringWithUTF8String:app->GetTitle()]];
    [g_window.object setDelegate:g_window.delegate];
    [g_window.object setAcceptsMouseMovedEvents:YES];

    if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
        [g_window.object setRestorable:NO];

    return true;
}

//------------------------------------------------------------------------
// Shtille Engine platform API
//------------------------------------------------------------------------

bool PlatformInit()
{
    g_window.need_quit = false;

    g_window.app_delegate = nil;

    g_window.autorelease_pool = [[NSAutoreleasePool alloc] init];

    g_window.event_source = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    if (!g_window.event_source)
        return false;

    CGEventSourceSetLocalEventsSuppressionInterval(g_window.event_source, 0.0);

    return true;
}
void PlatformTerminate()
{
    if (g_window.event_source)
    {
        CFRelease(g_window.event_source);
        g_window.event_source = NULL;
    }

    if (g_window.app_delegate)
    {
        [NSApp setDelegate:nil];
        [g_window.app_delegate release];
        g_window.app_delegate = nil;
    }

    [g_window.autorelease_pool release];
    g_window.autorelease_pool = nil;
}
bool PlatformWindowCreate()
{
    if (!InitializeAppKit())
        return false;

    if (!CreateWindow())
        return false;

    g_window.view = [[ShtilleEngineContentView alloc] init];

    [g_window.object setContentView:g_window.view];

    // Create a fullscreen window initially
    sht::Application * app = sht::Application::GetInstance();
    if (app->fullscreen())
        [g_window.object goFullscreen];

    return true;
}
void PlatformWindowDestroy()
{
    [g_window.object orderOut:nil];

    [g_window.object setDelegate:nil];
    [g_window.delegate release];
    g_window.delegate = nil;

    [g_window.view release];
    g_window.view = nil;

    [g_window.object close];
    g_window.object = nil;
}
bool PlatformNeedQuit()
{
    return g_window.need_quit;
}
void PlatformPollEvents()
{
#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200
    const NSEventMask kEventMaskAny = NSAnyEventMask;
#else
    const NSEventMask kEventMaskAny = NSEventMaskAny;
#endif
    while (true)
    {
        NSEvent* event = [NSApp nextEventMatchingMask:kEventMaskAny
                                            untilDate:[NSDate distantPast]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event == nil)
            break;

        [NSApp sendEvent:event];
    }

    [g_window.autorelease_pool drain];
    g_window.autorelease_pool = [[NSAutoreleasePool alloc] init];
}
void PlatformWindowMakeWindowed()
{
    [g_window.object goWindow];
}
bool PlatformWindowMakeFullscreen()
{
    [g_window.object goFullscreen];
    return true;
}
void PlatformWindowCenter()
{
    [g_window.object center];
}
void PlatformWindowResize(int width, int height)
{
    NSRect viewRect;
    viewRect.origin.x = 0;
    viewRect.origin.y = 0;
    viewRect.size.width = width;
    viewRect.size.height = height;
    NSRect rect = [g_window.view convertRectToBacking:viewRect];
    [g_window.object setFrame:rect display:YES animate:NO];
}
void PlatformWindowSetTitle(const char *title)
{
    [g_window.object setTitle:[NSString stringWithUTF8String:title]];
}
void PlatformWindowIconify()
{
    [g_window.object miniaturize:nil];
}
void PlatformWindowRestore()
{
    [g_window.object deminiaturize:nil];
}
void PlatformWindowShow()
{
    [g_window.object makeKeyAndOrderFront:nil];
}
void PlatformWindowHide()
{
    [g_window.object orderOut:nil];
}
void PlatformWindowTerminate()
{
    [[NSApplication sharedApplication] terminate:nil];
}
bool PlatformInitOpenGLContext(int color_bits, int depth_bits, int stencil_bits)
{
    sht::Application * app = sht::Application::GetInstance();

    std::vector<NSOpenGLPixelFormatAttribute> attributes;
    attributes.reserve(20);
    
    attributes.push_back(NSOpenGLPFADoubleBuffer);
    attributes.push_back(NSOpenGLPFAColorSize);
    attributes.push_back(app->color_bits());
    attributes.push_back(NSOpenGLPFADepthSize);
    attributes.push_back(app->depth_bits());
    attributes.push_back(NSOpenGLPFAStencilSize);
    attributes.push_back(app->stencil_bits());
    {
        // Must specify the 3.2 Core Profile to use OpenGL 3.2
        attributes.push_back(NSOpenGLPFAOpenGLProfile);
        attributes.push_back(NSOpenGLProfileVersion3_2Core);
    }
    if (app->IsMultisample())
    {
        // Enable multisampling
        attributes.push_back(NSOpenGLPFAMultisample);
        attributes.push_back(NSOpenGLPFASampleBuffers);
        attributes.push_back(1);
        attributes.push_back(NSOpenGLPFASamples);
        attributes.push_back(4);
    }
    attributes.push_back(0); // finishing sign
    
    NSOpenGLPixelFormatAttribute * attributes_ptr = &attributes[0];
    
    g_window.pixel_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes_ptr];
    
    if (g_window.pixel_format == nil)
    {
        NSLog(@"No OpenGL pixel format");
    }
       
    g_window.context = [[NSOpenGLContext alloc] initWithFormat:g_window.pixel_format shareContext:nil];
    
    // When we're using a CoreProfile context, crash if we call a legacy OpenGL function
    // This will make it much more obvious where and when such a function call is made so
    // that we can remove such calls.
    // Without this we'd simply get GL_INVALID_OPERATION error for calling legacy functions
    // but it would be more difficult to see where that function was called.
    CGLEnable([g_window.context CGLContextObj], kCGLCECrashOnRemovedFunctions);
    
    [g_window.view setPixelFormat:g_window.pixel_format];
    
    [g_window.view setOpenGLContext:g_window.context];
    
    // Opt-In to Retina resolution
    if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
        [g_window.view setWantsBestResolutionOpenGLSurface:YES];

    [g_window.context setView:g_window.view];

    return true;
}
void PlatformDeinitOpenGLContext()
{
    [g_window.pixel_format release];
    g_window.pixel_format = nil;

    [g_window.context release];
    g_window.context = nil;
}
void PlatformSwapBuffers()
{
    [g_window.context flushBuffer];
}
void PlatformMakeContextCurrent()
{
    [g_window.context makeCurrentContext];
}
void PlatformSwapInterval(int interval)
{
    GLint swapInt = (GLint)interval;
    [g_window.context setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
}
void PlatformSetCursorPos(float x, float y)
{
    const NSRect globalRect = [g_window.object convertRectToScreen:NSMakeRect(x, y, 0, 0)];
    const float displayHeight = CGDisplayBounds(CGMainDisplayID()).size.height;
    CGWarpMouseCursorPosition(CGPointMake(globalRect.origin.x, displayHeight - globalRect.origin.y));
}
void PlatformGetCursorPos(float& x, float& y)
{
    const NSPoint pos = [g_window.object mouseLocationOutsideOfEventStream];
    x = pos.x;
    y = pos.y;
}
void PlatformMouseToCenter()
{
    const NSRect frameRect = [g_window.object frame];
    NSRect contentRect = [g_window.object contentRectForFrameRect:frameRect];
    contentRect.origin.x = contentRect.size.width/2;
    contentRect.origin.y = contentRect.size.height/2;
    const NSRect globalRect = [g_window.object convertRectToScreen:contentRect];
    const float displayHeight = CGDisplayBounds(CGMainDisplayID()).size.height;
    CGWarpMouseCursorPosition(CGPointMake(globalRect.origin.x, displayHeight - globalRect.origin.y));
}
void PlatformShowCursor()
{
    [NSCursor unhide];
}
void PlatformHideCursor()
{
    [NSCursor hide];
}
void PlatformSetClipboardText(const char *text)
{
    NSArray* types = [NSArray arrayWithObjects:NSStringPboardType, nil];
    
    NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
    [pasteboard declareTypes:types owner:nil];
    [pasteboard setString:[NSString stringWithUTF8String:text]
                  forType:NSStringPboardType];
}
std::string PlatformGetClipboardText()
{
    NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
    
    if (![[pasteboard types] containsObject:NSStringPboardType])
    {
        NSLog(@"Cocoa: Failed to retrieve string from pasteboard");
        return NULL;
    }
    
    NSString* object = [pasteboard stringForType:NSStringPboardType];
    if (!object)
    {
        NSLog(@"Cocoa: Failed to retrieve object from pasteboard");
        return NULL;
    }
    
    std::string string = [object UTF8String];
    
    return string;
}
void PlatformChangeDirectoryToResources()
{
    char resourcesPath[MAXPATHLEN];
    
    CFBundleRef bundle = CFBundleGetMainBundle();
    if (!bundle)
        return;
    
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(bundle);
    
    CFStringRef last = CFURLCopyLastPathComponent(resourcesURL);
    if (CFStringCompare(CFSTR("Resources"), last, 0) != kCFCompareEqualTo)
    {
        CFRelease(last);
        CFRelease(resourcesURL);
        return;
    }
    
    CFRelease(last);
    
    if (!CFURLGetFileSystemRepresentation(resourcesURL,
                                          true,
                                          (UInt8*) resourcesPath,
                                          MAXPATHLEN))
    {
        CFRelease(resourcesURL);
        return;
    }
    
    CFRelease(resourcesURL);
    
    chdir(resourcesPath);
}