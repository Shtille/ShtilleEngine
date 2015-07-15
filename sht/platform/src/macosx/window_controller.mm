/*
     File: GLEssentialsWindowController.m
 Abstract: 
 Window controller class. Necessary to switch back and forth between a fullscreen and non-fullscreen window.
 The window property on NSWindowController is connected to the window in the NIB (via Interface Builder).
 
  Version: 1.7
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
 Inc. ("Apple") in consideration of your agreement to the following
 terms, and your use, installation, modification or redistribution of
 this Apple software constitutes acceptance of these terms.  If you do
 not agree with these terms, please do not use, install, modify or
 redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc. may
 be used to endorse or promote products derived from the Apple Software
 without specific prior written permission from Apple.  Except as
 expressly stated in this notice, no other rights or licenses, express or
 implied, are granted by Apple herein, including but not limited to any
 patent rights that may be infringed by your derivative works or by other
 works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2013 Apple Inc. All Rights Reserved.
 
 */

#import "window_controller.h"
#import "fullscreen_window.h"

#include "window_controller_interface.h"

#include "../../../application/application.h"

#ifdef __cplusplus
void * g_window_controller;
#else
GLEssentialsWindowController * g_window_controller;
#endif

@interface GLEssentialsWindowController ()

@end

@implementation GLEssentialsWindowController

// Were we set in the background?
BOOL wasBackgroundedOutOfFullScreen;


// Fullscreen window 
GLEssentialsFullscreenWindow *fullscreenWindow;

// Non-Fullscreen window (also the initial window)
NSWindow* standardWindow;

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];

	if (self)
	{
        // Store pointer to this instance
        g_window_controller = self;
		// Initialize to nil since it indicates app is not fullscreen
		fullscreenWindow = nil;
    }

	return self;
}

- (void) awakeFromNib
{
    // Make window non-resizable (!!! it's important, without this resize won't work)
    [self.window setStyleMask:[self.window styleMask] & ~NSResizableWindowMask];
    
    sht::Application * app = sht::Application::GetInstance();
    NSRect viewRect;
    viewRect.origin.x = 0;
    viewRect.origin.y = 0;
    viewRect.size.width = app->width();
    viewRect.size.height = app->height();
    
    // Set the view rect to the new size
    if (app->fullscreen())
    {
        NSRect rect = [view convertRectToBacking:viewRect];
        [self.window setFrame:rect display:NO animate:NO];
        [self goFullscreen];
    }
    else
    {
        // Windowed by default
        NSRect rect = [view convertRectToBacking:viewRect];
        [self.window setFrame:rect display:YES animate:NO];
        // TODO:
        //[self.window center];
    }
    // Set window title
    [self.window setTitle:[NSString stringWithUTF8String:app->GetTitle()]];
    
    // Now window may catch messages
    app->set_visible(true);
}

- (void) goFullscreen
{
	// If app is already fullscreen...
	if(fullscreenWindow)
	{
		//...don't do anything
		return;
	}

	// Allocate a new fullscreen window
	fullscreenWindow = [[GLEssentialsFullscreenWindow alloc] init];

	// Resize the view to screensize
	NSRect viewRect = [fullscreenWindow frame];

	// Set the view to the size of the fullscreen window
	[view setFrameSize: viewRect.size];

	// Set the view in the fullscreen window
	[fullscreenWindow setContentView:view];

	standardWindow = [self window];

	// Hide non-fullscreen window so it doesn't show up when switching out
	// of this app (i.e. with CMD-TAB)
	[standardWindow orderOut:self];

	// Set controller to the fullscreen window so that all input will go to
	// this controller (self)
	[self setWindow:fullscreenWindow];

	// Show the window and make it the key window for input
	[fullscreenWindow makeKeyAndOrderFront:self];

}

- (void) goWindow
{
	// If controller doesn't have a full screen window...
	if(fullscreenWindow == nil)
	{
		//...app is already windowed so don't do anything
		return;
	}

	// Get the rectangle of the original window
	NSRect viewRect = [standardWindow frame];
	
	// Set the view rect to the new size
	[view setFrame:viewRect];

	// Set controller to the standard window so that all input will go to
	// this controller (self)
	[self setWindow:standardWindow];

	// Set the content of the orginal window to the view
	[[self window] setContentView:view];

	// Show the window and make it the key window for input
	[[self window] makeKeyAndOrderFront:self];

	// Release the fullscreen window
	[fullscreenWindow release];

	// Ensure we set fullscreen Window to nil so our checks for 
	// windowed vs. fullscreen mode elsewhere are correct
	fullscreenWindow = nil;
}
- (void) doResize:(int) width
        andHeight:(int) height
{
    NSRect viewRect;
    viewRect.origin.x = 0;
    viewRect.origin.y = 0;
    viewRect.size.width = width;
    viewRect.size.height = height;
    NSRect rect = [view convertRectToBacking:viewRect];
    [self.window setFrame:rect display:YES animate:NO];
}

void PlatformWindowMakeWindowedImpl(void *instance)
{
    [(id) instance goWindow];
}
void PlatformWindowMakeFullscreenImpl(void *instance)
{
    [(id) instance goFullscreen];
}
void PlatformWindowResizeImpl(void *instance, int width, int height)
{
    if (fullscreenWindow != nil) return;
    [(id) instance doResize:width andHeight:height];
}
void PlatformWindowSetTitleImpl(void *instance, const char *title)
{
    [(id) instance setTitle:[NSString stringWithUTF8String:title]];
}
void PlatformWindowIconifyImpl(void *instance)
{
    [(id) instance miniaturize:nil];
}
void PlatformWindowRestoreImpl(void *instance)
{
    [(id) instance deminiaturize:nil];
}
void PlatformWindowShowImpl(void *instance)
{
    [(id) instance makeKeyAndOrderFront:nil];
}
void PlatformWindowHideImpl(void *instance)
{
    [(id) instance orderOut:nil];
}
void PlatformWindowTerminateImpl(void *instance)
{
    [[NSApplication sharedApplication] terminate:nil];
}
void PlatformSetCursorPosImpl(void *instance, int x, int y)
{
    // contentRect is needed when use different coord system
    //const NSRect contentRect = [(id) instance contentRectForFrameRect:[(id) instance frame]];
    const NSPoint localPoint = NSMakePoint(x, y);
    const NSPoint globalPoint = [(id) instance convertBaseToScreen:localPoint];
    CGWarpMouseCursorPosition(globalPoint);
}
void PlatformGetCursorPosImpl(void *instance, int& x, int& y)
{
    const NSPoint pos = [(id) instance mouseLocationOutsideOfEventStream];
    x = pos.x;
    y = pos.y;
}
void PlatformMouseToCenterImpl(void *instance)
{
    const NSRect contentRect = [(id) instance contentRectForFrameRect:[(id) instance frame]];
    const NSPoint localPoint = NSMakePoint(contentRect.origin.x + contentRect.size.width/2,
                                           contentRect.origin.y + contentRect.size.height/2);
    const NSPoint globalPoint = [(id) instance convertBaseToScreen:localPoint];
    CGWarpMouseCursorPosition(globalPoint);
}
void PlatformSetClipboardTextImpl(void *instance, const char *text)
{
    NSArray* types = [NSArray arrayWithObjects:NSStringPboardType, nil];
    
    NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
    [pasteboard declareTypes:types owner:nil];
    [pasteboard setString:[NSString stringWithUTF8String:text]
                  forType:NSStringPboardType];
}
const char* PlatformGetClipboardTextImpl(void *instance)
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
    
    const char *string = strdup([object UTF8String]);
    
    return string;
}

//- (BOOL) acceptsFirstResponder
//{
//    return YES;
//}

//- (void) keyDown:(NSEvent *)event
//{
//    // TODO: take into account key modifiers
//	unichar c = [[event charactersIgnoringModifiers] characterAtIndex:0];
//    //unichar c = [event keyCode];
//    
//    sht::Application * app = sht::Application::GetInstance();
//    if (app->OnKeyDown(c))
//        return;
//
//	// Allow other character to be handled (or not and beep)
//	[super keyDown:event];
//}
//
//- (void) keyUp:(NSEvent *)event
//{
//    unichar c = [[event charactersIgnoringModifiers] characterAtIndex:0];
//    
//    sht::Application * app = sht::Application::GetInstance();
//    if (app->OnKeyUp(c))
//        return;
//    
//    // Allow other character to be handled (or not and beep)
//    [super keyUp:event];
//}

@end
