#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "MainComponent.h"

class XYControlAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    XYControlAudioProcessorEditor(XYControlAudioProcessor&);
    ~XYControlAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    XYControlAudioProcessor& audioProcessor;
    MainComponent mainComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYControlAudioProcessorEditor)
};
