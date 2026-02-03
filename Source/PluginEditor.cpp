#include "PluginProcessor.h"
#include "PluginEditor.h"

XYControlAudioProcessorEditor::XYControlAudioProcessorEditor(XYControlAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    addAndMakeVisible(mainComponent);
    setSize(700, 700);
}

XYControlAudioProcessorEditor::~XYControlAudioProcessorEditor()
{
}

void XYControlAudioProcessorEditor::paint(juce::Graphics& g)
{
    // MainComponent handles all painting
}

void XYControlAudioProcessorEditor::resized()
{
    mainComponent.setBounds(getLocalBounds());
}
