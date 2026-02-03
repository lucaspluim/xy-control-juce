#include "PluginProcessor.h"
#include "PluginEditor.h"

XYControlAudioProcessorEditor::XYControlAudioProcessorEditor (XYControlAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(mainComponent);
    setSize (400, 450);
    
    // Restore XY position from processor
    mainComponent.setXYPosition(audioProcessor.getXYPosition());
}

XYControlAudioProcessorEditor::~XYControlAudioProcessorEditor()
{
    // Save XY position to processor
    audioProcessor.setXYPosition(mainComponent.getXYPosition());
}

void XYControlAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void XYControlAudioProcessorEditor::resized()
{
    mainComponent.setBounds(getLocalBounds());
}
