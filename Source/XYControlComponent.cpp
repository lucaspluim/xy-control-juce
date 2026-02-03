#include "XYControlComponent.h"
#include "BinaryData.h"

XYControlComponent::XYControlComponent()
    : springLayers{{
        SpringLayer(0.20f, 1.13f, 1.6f),    // cursor - overdamped, zero bounce
        SpringLayer(0.09f, 0.88f, 3.8f),    // inner
        SpringLayer(0.07f, 0.85f, 5.2f),    // mid
        SpringLayer(0.05f, 0.82f, 6.8f),    // outer
        SpringLayer(0.04f, 0.78f, 8.5f),    // ambient
        SpringLayer(0.03f, 0.75f, 10.5f)    // atmosphere
    }},
    glowLayers{{
        {120, 0.95f, juce::Colour::fromFloatRGBA(0.0f, 0.55f, 1.0f, 0.95f), {}},
        {180, 0.75f, juce::Colour::fromFloatRGBA(0.0f, 0.57f, 1.0f, 0.75f), {}},
        {260, 0.60f, juce::Colour::fromFloatRGBA(0.04f, 0.59f, 1.0f, 0.60f), {}},
        {360, 0.45f, juce::Colour::fromFloatRGBA(0.12f, 0.63f, 1.0f, 0.45f), {}},
        {480, 0.35f, juce::Colour::fromFloatRGBA(0.20f, 0.69f, 1.0f, 0.35f), {}}
    }}
{
    lastFrameTime = juce::Time::currentTimeMillis();
    startTimerHz(60);

    loadGlowImagesFromBinaryData();
}

XYControlComponent::~XYControlComponent()
{
}

void XYControlComponent::loadGlowImagesFromBinaryData()
{
    // Load pre-rendered PNG images from binary data
    const char* resourceNames[] = {
        "glow_layer_0_png",
        "glow_layer_1_png",
        "glow_layer_2_png",
        "glow_layer_3_png",
        "glow_layer_4_png"
    };

    for (size_t i = 0; i < glowLayers.size(); ++i)
    {
        // Get the binary data for this layer
        int dataSize = 0;
        const char* data = BinaryData::getNamedResource(resourceNames[i], dataSize);

        if (data != nullptr && dataSize > 0)
        {
            // Load PNG from memory
            glowLayers[i].cachedImage = juce::ImageFileFormat::loadFrom(data, (size_t)dataSize);
        }
        else
        {
            // Fallback: create a simple colored circle if resource missing
            juce::Image fallback(juce::Image::ARGB, glowLayers[i].size, glowLayers[i].size, true);
            juce::Graphics g(fallback);
            g.setColour(glowLayers[i].color);
            g.fillEllipse(0, 0, (float)glowLayers[i].size, (float)glowLayers[i].size);
            glowLayers[i].cachedImage = fallback;
        }
    }
}

void XYControlComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Draw white rounded rectangle background
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(bounds.toFloat(), 24.0f);

    // Enable high quality rendering
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);

    // Clip to rounded rectangle
    juce::Path clipPath;
    clipPath.addRoundedRectangle(bounds.toFloat(), 24.0f);
    g.reduceClipRegion(clipPath);

    // Draw glow layers from back to front
    for (int i = 4; i >= 0; --i)
    {
        auto& spring = springLayers[i + 1];
        auto& layer = glowLayers[i];

        float pixelX = spring.x * bounds.getWidth();
        float pixelY = spring.y * bounds.getHeight();

        float scaleX = 1.0f;
        float scaleY = 1.0f;
        float offsetX = 0.0f;
        float offsetY = 0.0f;
        float rotation = 0.0f;
        float opacity = layer.opacity;

        if (isBreathing)
        {
            // Breathing animation with slightly different timing for each layer
            float breathePhase = breatheTime + i * 0.3f;
            float breatheScale = 1.0f + 0.08f * std::sin(breathePhase);
            scaleX = breatheScale;
            scaleY = breatheScale;
            opacity *= 0.85f + 0.15f * (0.5f + 0.5f * std::sin(breathePhase));
        }
        else
        {
            // Calculate velocity magnitude and direction
            float speed = std::sqrt(spring.vx * spring.vx + spring.vy * spring.vy);

            // Always calculate based on velocity, but smoothly fade to neutral
            if (speed > 0.0001f)
            {
                // Angle of movement
                rotation = std::atan2(spring.vy, spring.vx);

                // Speed-based stretching factor with smooth falloff
                float speedFactor = 1.0f - std::exp(-speed * 8.0f);

                // Create comet tail effect:
                // - Stretch along direction of movement (scaleX)
                // - Squash perpendicular (scaleY)
                // - More dramatic on outer layers
                float stretchMultiplier = 1.0f + i * 0.3f;
                scaleX = 1.0f + speedFactor * (1.2f + stretchMultiplier);  // Stretch behind
                scaleY = 1.0f / (1.0f + speedFactor * (0.5f + i * 0.1f)); // Squash sides

                // Offset layers backward along movement vector for tail effect
                float offsetAmount = speedFactor * (15.0f + i * 8.0f);
                offsetX = -std::cos(rotation) * offsetAmount;
                offsetY = -std::sin(rotation) * offsetAmount;
            }
        }

        // Draw the cached blurred image with comet transformation
        g.setOpacity(opacity);

        // Create proper directional stretch transform
        float centerX = layer.cachedImage.getWidth() / 2.0f;
        float centerY = layer.cachedImage.getHeight() / 2.0f;

        juce::AffineTransform transform = juce::AffineTransform()
            .translated(-centerX, -centerY)                    // Center at origin
            .scaled(scaleX, scaleY)                            // Apply scale
            .followedBy(juce::AffineTransform::rotation(rotation)) // Rotate
            .translated(pixelX + offsetX, pixelY + offsetY);   // Move to position

        g.drawImageTransformed(layer.cachedImage, transform, false);
    }

    // Draw solid white cursor
    g.setOpacity(1.0f);

    float cursorX = springLayers[0].x * bounds.getWidth();
    float cursorY = springLayers[0].y * bounds.getHeight();
    float cursorRadius = isDragging ? 8.0f : 9.0f;

    // Solid white circle
    g.setColour(juce::Colours::white);
    g.fillEllipse(cursorX - cursorRadius, cursorY - cursorRadius,
                  cursorRadius * 2, cursorRadius * 2);
}

void XYControlComponent::resized()
{
}

void XYControlComponent::mouseDown(const juce::MouseEvent& event)
{
    // Immediately stop breathing to prevent jitter
    isBreathing = false;
    idleTimer = 0.0f;
    isDragging = true;

    auto bounds = getLocalBounds().toFloat();
    targetX = event.position.x / bounds.getWidth();
    targetY = event.position.y / bounds.getHeight();

    repaint();
}

void XYControlComponent::mouseDrag(const juce::MouseEvent& event)
{
    // Ensure breathing is stopped during any drag
    isBreathing = false;
    idleTimer = 0.0f;

    auto bounds = getLocalBounds().toFloat();
    targetX = juce::jlimit(0.0f, 1.0f, event.position.x / bounds.getWidth());
    targetY = juce::jlimit(0.0f, 1.0f, event.position.y / bounds.getHeight());

    repaint();
}

void XYControlComponent::mouseUp(const juce::MouseEvent&)
{
    // Don't immediately set isDragging to false - let motion settle first
    // This prevents sudden changes when releasing
    isDragging = false;

    // Don't reset idle timer - let it accumulate naturally
    // idleTimer will start when velocity drops below threshold

    repaint();
}

void XYControlComponent::timerCallback()
{
    int64_t currentTime = juce::Time::currentTimeMillis();
    float dt = juce::jmin((currentTime - lastFrameTime) / 16.67f, 2.0f);
    lastFrameTime = currentTime;

    // Update all spring layers
    springLayers[0].update(targetX, targetY, dt);

    for (size_t i = 1; i < springLayers.size(); ++i)
    {
        springLayers[i].update(springLayers[0].x, springLayers[0].y, dt);

        // Very gradual velocity decay for smoothest settling
        // Only decay when extremely small to prevent any snapping
        if (std::abs(springLayers[i].vx) < 0.0005f)
            springLayers[i].vx *= 0.98f;  // More gradual
        if (std::abs(springLayers[i].vy) < 0.0005f)
            springLayers[i].vy *= 0.98f;

        // Only fully zero out when microscopic
        if (std::abs(springLayers[i].vx) < 0.00001f)
            springLayers[i].vx = 0.0f;
        if (std::abs(springLayers[i].vy) < 0.00001f)
            springLayers[i].vy = 0.0f;
    }

    // Check for idle state - use blur layers to determine true stillness
    float totalVelocity = std::abs(springLayers[0].vx) + std::abs(springLayers[0].vy);
    float blurVelocity = 0.0f;
    for (size_t i = 1; i < springLayers.size(); ++i)
    {
        blurVelocity += std::abs(springLayers[i].vx) + std::abs(springLayers[i].vy);
    }

    if (totalVelocity < 0.001f && blurVelocity < 0.01f && !isDragging)
    {
        idleTimer += dt * 16.67f;
        if (idleTimer > 500.0f)  // Longer delay before breathing starts
        {
            if (!isBreathing)
            {
                // Start breathing at neutral phase (where sin = 0) to avoid snap
                breatheTime = 0.0f;
                isBreathing = true;
            }
        }
    }
    else
    {
        idleTimer = 0.0f;
    }

    // Update breathing animation time
    if (isBreathing)
    {
        breatheTime += 0.025f;
    }

    repaint();
}
