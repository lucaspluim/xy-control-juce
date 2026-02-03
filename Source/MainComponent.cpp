#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(700, 700);
    addAndMakeVisible(xyControl);

    // Get or create presets folder
    presetsFolder = NativeDialogs::getPresetsFolder();

    // Optimize for smooth animation
    setOpaque(true);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colour(0xfff5f5f7));

    // Draw drop shadow for XY control with appropriate color for preset
    auto controlBounds = xyControl.getBounds();
    juce::Path shadowPath;
    shadowPath.addRoundedRectangle(controlBounds.toFloat(), 24.0f);

    // Use lighter shadow for dark presets, darker shadow for light presets
    auto preset = xyControl.getCurrentPreset();
    juce::Colour baseShadowColor;

    if (preset == XYControlComponent::Preset::Blue)
        baseShadowColor = juce::Colour(0x14000000);  // Dark shadow for white background
    else if (preset == XYControlComponent::Preset::Red)
        baseShadowColor = juce::Colour(0x30000000);  // Darker shadow for red
    else // Black
        baseShadowColor = juce::Colour(0x40000000);  // Even darker shadow for black

    // Normal shadow always (doesn't grow)
    juce::DropShadow shadow(baseShadowColor, 16, juce::Point<int>(0, 4));
    shadow.drawForPath(g, shadowPath);

    if (holdProgress > 0.0f)
    {
        // Only animate the blue ring - keep it simple for smoothness
        float ringAlpha = 0.2f + holdProgress * 0.5f;
        g.setColour(juce::Colour(0xff007aff).withAlpha(ringAlpha));

        // Use float directly for stroke and expansion - no casting
        float strokeWidth = 2.0f + holdProgress * 4.0f;
        float expansion = 6.0f + holdProgress * 24.0f;

        g.drawRoundedRectangle(controlBounds.toFloat().expanded(expansion),
                              28.0f, strokeWidth);
    }
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    // Make it 500x500 like the HTML version
    xyControl.setBounds(bounds.withSizeKeepingCentre(500, 500));
}

void MainComponent::mouseDown(const juce::MouseEvent& event)
{
    // Check if click is outside the XY control area
    if (!xyControl.getBounds().contains(event.getPosition()))
    {
        isHoldingOutside = true;
        holdStartTime = juce::Time::currentTimeMillis();
        holdProgress = 0.0f;
        menuShown = false;
        startTimerHz(60);  // Explicit 60Hz for smooth animation
    }
}

void MainComponent::mouseUp(const juce::MouseEvent& event)
{
    isHoldingOutside = false;
    holdProgress = 0.0f;
    stopTimer();
    repaint();
}

void MainComponent::mouseDoubleClick(const juce::MouseEvent& event)
{
    // Check if double-click is outside the XY control area
    if (!xyControl.getBounds().contains(event.getPosition()))
    {
        // Cycle to next preset
        auto currentPreset = static_cast<int>(xyControl.getCurrentPreset());
        currentPreset = (currentPreset + 1) % 3;  // 0->1->2->0
        xyControl.setPreset(static_cast<XYControlComponent::Preset>(currentPreset));
    }
}

void MainComponent::timerCallback()
{
    if (isHoldingOutside && !menuShown)
    {
        int64_t currentTime = juce::Time::currentTimeMillis();
        int64_t holdDuration = currentTime - holdStartTime;

        // Update hold progress for visual feedback - use float division for smoothness
        holdProgress = juce::jmin(1.0f, (float)holdDuration / 3000.0f);

        // Force immediate repaint for smooth animation
        repaint();

        if (holdDuration >= 3000)  // 3 seconds
        {
            menuShown = true;
            holdProgress = 0.0f;
            stopTimer();
            repaint();

            showPresetOptions();
        }
    }
    else if (holdProgress > 0.0f)
    {
        // Continue animating even after release until fully reset
        repaint();
    }
}

void MainComponent::showPresetOptions()
{
    NativeDialogs::showPresetOptions(presetsFolder,
        [this](juce::File file) {
            // Save
            if (file.existsAsFile() || file.getParentDirectory().exists()) {
                savePresetToFile(file);
            }
        },
        [this](juce::File file) {
            // Load
            if (file.existsAsFile()) {
                loadPresetFromFile(file);
            }
        });
}


void MainComponent::savePresetToFile(const juce::File& file)
{
    // Create JSON preset data
    juce::var presetData(new juce::DynamicObject());
    auto* obj = presetData.getDynamicObject();

    obj->setProperty("version", 1);
    obj->setProperty("preset_type", (int)xyControl.getCurrentPreset());
    obj->setProperty("name", file.getFileNameWithoutExtension());

    // Write to file
    juce::String jsonString = juce::JSON::toString(presetData, true);
    bool success = file.replaceWithText(jsonString);

    if (success) {
        NativeDialogs::showConfirmation("Preset Saved",
                                       "\"" + file.getFileNameWithoutExtension() + "\" saved successfully!",
                                       [](){});
    }
}

void MainComponent::loadPresetFromFile(const juce::File& file)
{
    if (!file.existsAsFile())
        return;

    // Read JSON from file
    juce::String jsonString = file.loadFileAsString();
    juce::var presetData = juce::JSON::parse(jsonString);

    if (presetData.isObject())
    {
        auto* obj = presetData.getDynamicObject();
        if (obj->hasProperty("preset_type"))
        {
            int presetType = obj->getProperty("preset_type");
            xyControl.setPreset(static_cast<XYControlComponent::Preset>(presetType));

            NativeDialogs::showConfirmation("Preset Loaded",
                                           "Loaded \"" + file.getFileNameWithoutExtension() + "\"",
                                           [](){});
        }
    }
}
