#pragma once

#include <JuceHeader.h>

class NativeDialogs
{
public:
    static juce::String saveFileDialog(const juce::String& title, const juce::String& extension);
    static juce::String openFileDialog(const juce::String& title, const juce::String& extension);
};
