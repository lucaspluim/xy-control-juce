#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(700, 700);
    addAndMakeVisible(xyControl);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colour(0xfff5f5f7));

    // Draw drop shadow for XY control
    auto controlBounds = xyControl.getBounds();
    juce::Path shadowPath;
    shadowPath.addRoundedRectangle(controlBounds.toFloat(), 24.0f);

    juce::DropShadow shadow(juce::Colour(0x14000000), 16, juce::Point<int>(0, 4));
    shadow.drawForPath(g, shadowPath);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    // Make it 500x500 like the HTML version
    xyControl.setBounds(bounds.withSizeKeepingCentre(500, 500));
}
