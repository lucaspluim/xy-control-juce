#include "XYControlComponent.h"
#include "BinaryData.h"

XYControlComponent::XYControlComponent()
    : springLayers{{
        SpringLayer(0.24f, 0.89f, 1.0f),
        SpringLayer(0.20f, 1.13f, 1.6f),
        SpringLayer(0.18f, 1.07f, 2.2f),
        SpringLayer(0.16f, 1.05f, 2.8f),
        SpringLayer(0.14f, 1.03f, 3.4f),
        SpringLayer(0.12f, 1.01f, 4.0f)
    }}
{
    loadGlowImagesFromBinaryData();
    startTimerHz(60);
}

XYControlComponent::~XYControlComponent()
{
}

void XYControlComponent::loadGlowImagesFromBinaryData()
{
    const char* presetName;
    int sizes[5];

    switch (currentPreset)
    {
        case Preset::Blue:
            presetName = "blue";
            sizes[0] = 120; sizes[1] = 180; sizes[2] = 260; sizes[3] = 360; sizes[4] = 480;
            break;
        case Preset::Red:
            presetName = "red";
            sizes[0] = 120; sizes[1] = 180; sizes[2] = 260; sizes[3] = 360; sizes[4] = 480;
            break;
        case Preset::Black:
            presetName = "black";
            sizes[0] = 100; sizes[1] = 150; sizes[2] = 215; sizes[3] = 300; sizes[4] = 400;
            break;
    }

    for (int i = 0; i < 5; ++i)
    {
        juce::String resourceName = juce::String("glow_") + presetName + "_layer_" + juce::String(i) + "_png";

        int dataSize = 0;
        const char* data = BinaryData::getNamedResource(resourceName.toRawUTF8(), dataSize);

        if (data != nullptr && dataSize > 0)
        {
            glowLayers[i] = juce::ImageCache::getFromMemory(data, dataSize);
        }
    }
}

void XYControlComponent::setPreset(Preset preset)
{
    if (currentPreset != preset)
    {
        currentPreset = preset;
        loadGlowImagesFromBinaryData();

        const float goldenAngle = 2.39996f;
        float baseAngle = juce::Random::getSystemRandom().nextFloat() * 6.28318f;

        for (size_t i = 1; i < springLayers.size(); ++i)
        {
            float angle = baseAngle + (i - 1) * goldenAngle;
            float dx = std::cos(angle);
            float dy = std::sin(angle);
            float impulse = 0.08f + i * 0.025f;

            springLayers[i].vx += dx * impulse;
            springLayers[i].vy += dy * impulse;
        }

        repaint();
    }
}

void XYControlComponent::paint(juce::Graphics& g)
{
    juce::Colour bgColor, gridColor, cursorColor, highlightColor;

    switch (currentPreset)
    {
        case Preset::Blue:
            bgColor = juce::Colours::white;
            gridColor = juce::Colour(0xffe0e0e0);
            cursorColor = juce::Colour(0xff007aff);
            highlightColor = juce::Colour(0x20007aff);
            break;
        case Preset::Red:
            bgColor = juce::Colour(0xff1a0000);
            gridColor = juce::Colour(0xff4a0000);
            cursorColor = juce::Colour(0xffff453a);
            highlightColor = juce::Colour(0x20ff453a);
            break;
        case Preset::Black:
            bgColor = juce::Colours::black;
            gridColor = juce::Colour(0xff2a2a2a);
            cursorColor = juce::Colours::white;
            highlightColor = juce::Colour(0x20ffffff);
            break;
    }

    g.fillAll(bgColor);

    auto bounds = getLocalBounds().toFloat();
    float cellSize = 50.0f;

    g.setColour(gridColor);
    for (float x = cellSize; x < bounds.getWidth(); x += cellSize)
        g.drawLine(x, 0, x, bounds.getHeight(), 1.0f);
    for (float y = cellSize; y < bounds.getHeight(); y += cellSize)
        g.drawLine(0, y, bounds.getWidth(), y, 1.0f);

    float centerX = bounds.getWidth() / 2.0f;
    float centerY = bounds.getHeight() / 2.0f;
    g.setColour(gridColor.withMultipliedAlpha(2.0f));
    g.drawLine(centerX, 0, centerX, bounds.getHeight(), 2.0f);
    g.drawLine(0, centerY, bounds.getWidth(), centerY, 2.0f);

    float breatheScale = 1.0f;
    if (!isDragging && breatheBlend > 0.0f)
    {
        float breatheAmount = std::sin(breathePhase) * 0.015f + 1.0f;
        breatheScale = 1.0f + (breatheAmount - 1.0f) * breatheBlend;
    }

    for (int i = 4; i >= 1; --i)
    {
        if (!glowLayers[i].isValid())
            continue;

        float layerX = springLayers[i + 1].x;
        float layerY = springLayers[i + 1].y;

        float screenX = layerX * bounds.getWidth();
        float screenY = layerY * bounds.getHeight();

        float imgW = glowLayers[i].getWidth() * breatheScale;
        float imgH = glowLayers[i].getHeight() * breatheScale;

        float drawX = screenX - imgW / 2.0f;
        float drawY = screenY - imgH / 2.0f;

        g.setOpacity(1.0f);
        g.drawImage(glowLayers[i],
                   juce::Rectangle<float>(drawX, drawY, imgW, imgH),
                   juce::RectanglePlacement::stretchToFit);
    }

    float ballX = cursorX * bounds.getWidth();
    float ballY = cursorY * bounds.getHeight();
    float ballRadius = 12.0f;

    if (!glowLayers[0].isValid())
    {
        g.setColour(cursorColor);
        g.fillEllipse(ballX - ballRadius, ballY - ballRadius, ballRadius * 2, ballRadius * 2);
    }
    else
    {
        float coreImgW = glowLayers[0].getWidth();
        float coreImgH = glowLayers[0].getHeight();
        float drawX = ballX - coreImgW / 2.0f;
        float drawY = ballY - coreImgH / 2.0f;

        g.setOpacity(1.0f);
        g.drawImage(glowLayers[0],
                   juce::Rectangle<float>(drawX, drawY, coreImgW, coreImgH),
                   juce::RectanglePlacement::stretchToFit);
    }

    if (isDragging)
    {
        g.setColour(highlightColor);
        g.fillEllipse(ballX - 50, ballY - 50, 100, 100);
    }
}

void XYControlComponent::resized()
{
}

void XYControlComponent::mouseDown(const juce::MouseEvent& event)
{
    isDragging = true;
    breatheBlend = 0.0f;

    auto bounds = getLocalBounds().toFloat();
    cursorX = juce::jlimit(0.0f, 1.0f, event.position.x / bounds.getWidth());
    cursorY = juce::jlimit(0.0f, 1.0f, event.position.y / bounds.getHeight());
    targetX = cursorX;
    targetY = cursorY;

    for (auto& layer : springLayers)
    {
        layer.x = cursorX;
        layer.y = cursorY;
        layer.vx = 0;
        layer.vy = 0;
    }

    repaint();
}

void XYControlComponent::mouseDrag(const juce::MouseEvent& event)
{
    auto bounds = getLocalBounds().toFloat();
    cursorX = juce::jlimit(0.0f, 1.0f, event.position.x / bounds.getWidth());
    cursorY = juce::jlimit(0.0f, 1.0f, event.position.y / bounds.getHeight());
    targetX = cursorX;
    targetY = cursorY;

    for (auto& layer : springLayers)
    {
        layer.x = cursorX;
        layer.y = cursorY;
        layer.vx = 0;
        layer.vy = 0;
    }

    repaint();
}

void XYControlComponent::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
    targetX = 0.5f;
    targetY = 0.5f;
}

void XYControlComponent::mouseDoubleClick(const juce::MouseEvent& event)
{
    const float goldenAngle = 2.39996f;
    float baseAngle = juce::Random::getSystemRandom().nextFloat() * 6.28318f;

    for (size_t i = 1; i < springLayers.size(); ++i)
    {
        float angle = baseAngle + (i - 1) * goldenAngle;
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        float impulse = 0.08f + i * 0.025f;

        springLayers[i].vx += dx * impulse;
        springLayers[i].vy += dy * impulse;
    }
}

void XYControlComponent::timerCallback()
{
    const float dt = 1.0f / 60.0f;

    if (!isDragging)
    {
        breathePhase += dt * 2.0f;
        breatheBlend = juce::jmin(1.0f, breatheBlend + dt * 1.0f);
    }

    for (auto& layer : springLayers)
        layer.update(targetX, targetY, dt);

    cursorX = springLayers[0].x;
    cursorY = springLayers[0].y;

    repaint();
}
