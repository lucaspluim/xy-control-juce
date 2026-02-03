#include "NativeDialogs.h"
#import <Cocoa/Cocoa.h>

juce::String NativeDialogs::saveFileDialog(const juce::String& title, const juce::String& extension)
{
    NSSavePanel* panel = [NSSavePanel savePanel];
    [panel setTitle:[NSString stringWithUTF8String:title.toRawUTF8()]];
    [panel setAllowedFileTypes:@[[NSString stringWithUTF8String:extension.toRawUTF8()]]];
    [panel setCanCreateDirectories:YES];
    
    if ([panel runModal] == NSModalResponseOK)
    {
        NSURL* url = [panel URL];
        NSString* path = [url path];
        return juce::String([path UTF8String]);
    }
    
    return juce::String();
}

juce::String NativeDialogs::openFileDialog(const juce::String& title, const juce::String& extension)
{
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    [panel setTitle:[NSString stringWithUTF8String:title.toRawUTF8()]];
    [panel setAllowedFileTypes:@[[NSString stringWithUTF8String:extension.toRawUTF8()]]];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories:NO];
    [panel setAllowsMultipleSelection:NO];
    
    if ([panel runModal] == NSModalResponseOK)
    {
        NSURL* url = [[panel URLs] objectAtIndex:0];
        NSString* path = [url path];
        return juce::String([path UTF8String]);
    }
    
    return juce::String();
}
