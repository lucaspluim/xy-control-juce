#include "XYControlComponent.h"

XYControlComponent::XYControlComponent()
    : animationTimer(*this)
{
    setSize(400, 400);
    animationTimer.startTimerHz(60); // 60 FPS
}

XYControlComponent::~XYControlComponent()
{
    animationTimer.stopTimer();
}

void XYControlComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Background
    g.fillAll(juce::Colour(0xff0a0a0a));
    
    // Get color scheme colors
    juce::Colour primaryColor, glowColor;
    
    switch (currentColorScheme)
    {
        case 0: // Blue
            primaryColor = juce::Colour(0xff00bfff);
            glowColor = juce::Colour(0xff0080ff);
            break;
        case 1: // Red
            primaryColor = juce::Colour(0xffff4444);
            glowColor = juce::Colour(0xffff0000);
            break;
        case 2: // Black
            primaryColor = juce::Colour(0xff333333);
            glowColor = juce::Colour(0xff666666);
            break;
        default:
            primaryColor = juce::Colour(0xff00bfff);
            glowColor = juce::Colour(0xff0080ff);
            break;
    }
    
    // Grid
    g.setColour(juce::Colour(0xff222222));
    for (int i = 1; i < 4; ++i)
    {
        float pos = bounds.getWidth() * i / 4.0f;
        g.drawLine(pos, 0, pos, bounds.getHeight(), 1.0f);
        g.drawLine(0, pos, bounds.getWidth(), pos, 1.0f);
    }
    
    // Center crosshair
    g.setColour(juce::Colour(0xff444444));
    float centerX = bounds.getWidth() / 2.0f;
    float centerY = bounds.getHeight() / 2.0f;
    g.drawLine(centerX - 10, centerY, centerX + 10, centerY, 1.0f);
    g.drawLine(centerX, centerY - 10, centerX, centerY + 10, 1.0f);
    
    // Calculate actual position on screen
    float x = position.x * bounds.getWidth();
    float y = position.y * bounds.getHeight();
    
    // Breathing effect
    float breathingScale = 1.0f + 0.05f * std::sin(breathingPhase);
    float baseRadius = 20.0f * breathingScale;
    
    // Hold indicator (blue ring that grows)
    if (isHolding && holdProgress > 0.0f)
    {
        float ringRadius = baseRadius + 15.0f * holdProgress;
        g.setColour(juce::Colour(0xff00bfff).withAlpha(0.3f * (1.0f - holdProgress * 0.5f)));
        g.drawEllipse(x - ringRadius, y - ringRadius, ringRadius * 2, ringRadius * 2, 3.0f);
    }
    
    // Disperse particles
    if (isDispersing)
    {
        g.setColour(primaryColor.withAlpha(1.0f - disperseProgress));
        for (auto& particle : disperseParticles)
        {
            float px = particle.x * bounds.getWidth();
            float py = particle.y * bounds.getHeight();
            float particleSize = 8.0f * (1.0f - disperseProgress);
            g.fillEllipse(px - particleSize / 2, py - particleSize / 2, particleSize, particleSize);
        }
    }
    
    // Glow effect (multiple layers)
    if (!isDispersing)
    {
        for (int i = 3; i > 0; --i)
        {
            float glowRadius = baseRadius + i * 8.0f;
            float alpha = 0.15f / i;
            g.setColour(glowColor.withAlpha(alpha));
            g.fillEllipse(x - glowRadius, y - glowRadius, glowRadius * 2, glowRadius * 2);
        }
        
        // Main ball
        g.setColour(primaryColor);
        g.fillEllipse(x - baseRadius, y - baseRadius, baseRadius * 2, baseRadius * 2);
        
        // Highlight
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        float highlightRadius = baseRadius * 0.4f;
        g.fillEllipse(x - baseRadius * 0.4f, y - baseRadius * 0.4f, 
                     highlightRadius * 2, highlightRadius * 2);
    }
    
    // Coordinates display
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(12.0f);
    juce::String coordText = juce::String::formatted("X: %.2f  Y: %.2f", position.x, position.y);
    g.drawText(coordText, bounds.reduced(10), juce::Justification::topRight);
    
    // Color scheme indicator
    juce::String schemeText;
    switch (currentColorScheme)
    {
        case 0: schemeText = "Blue"; break;
        case 1: schemeText = "Red"; break;
        case 2: schemeText = "Black"; break;
    }
    g.drawText(schemeText, bounds.reduced(10), juce::Justification::topLeft);
}

void XYControlComponent::resized()
{
}

void XYControlComponent::mouseDown(const juce::MouseEvent& event)
{
    // Check for double-click
    int64 currentTime = juce::Time::currentTimeMillis();
    if (currentTime - lastClickTime < doubleClickThreshold)
    {
        // Double-click detected
        cycleColorScheme();
        triggerDisperse();
        lastClickTime = 0; // Reset to prevent triple-click
    }
    else
    {
        lastClickTime = currentTime;
        isDragging = true;
        
        // Update position immediately
        auto bounds = getLocalBounds().toFloat();
        position.x = juce::jlimit(0.0f, 1.0f, event.position.x / bounds.getWidth());
        position.y = juce::jlimit(0.0f, 1.0f, event.position.y / bounds.getHeight());
        targetPosition = position;
        velocity = juce::Point<float>(0.0f, 0.0f);
        
        startHoldTimer();
        repaint();
    }
}

void XYControlComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (isDragging)
    {
        auto bounds = getLocalBounds().toFloat();
        position.x = juce::jlimit(0.0f, 1.0f, event.position.x / bounds.getWidth());
        position.y = juce::jlimit(0.0f, 1.0f, event.position.y / bounds.getHeight());
        targetPosition = position;
        velocity = juce::Point<float>(0.0f, 0.0f);
        
        // Reset hold timer if moved
        stopHoldTimer();
        
        repaint();
    }
}

void XYControlComponent::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
    stopHoldTimer();
    
    // Spring back to center
    targetPosition = juce::Point<float>(0.5f, 0.5f);
}

void XYControlComponent::updatePhysics()
{
    // Update breathing animation
    breathingPhase += 0.05f;
    if (breathingPhase > juce::MathConstants<float>::twoPi)
        breathingPhase -= juce::MathConstants<float>::twoPi;
    
    // Update hold progress
    if (isHolding)
    {
        int64 currentTime = juce::Time::currentTimeMillis();
        int64 elapsed = currentTime - holdStartTime;
        holdProgress = juce::jlimit(0.0f, 1.0f, elapsed / (float)holdDuration);
    }
    
    // Update disperse effect
    if (isDispersing)
    {
        disperseProgress += 0.02f;
        if (disperseProgress >= 1.0f)
        {
            isDispersing = false;
            disperseProgress = 0.0f;
        }
        else
        {
            // Update particle positions
            for (int i = 0; i < disperseParticles.size(); ++i)
            {
                disperseParticles.getReference(i).x += disperseVelocities[i].x * 0.016f;
                disperseParticles.getReference(i).y += disperseVelocities[i].y * 0.016f;
            }
        }
    }
    
    // Spring physics (only when not dragging)
    if (!isDragging)
    {
        const float springStiffness = 0.1f;
        const float damping = 0.8f;
        
        juce::Point<float> displacement = targetPosition - position;
        juce::Point<float> springForce = displacement * springStiffness;
        
        velocity += springForce;
        velocity *= damping;
        
        position += velocity;
        
        // Stop if very close to target
        if (displacement.getDistanceFromOrigin() < 0.001f && 
            velocity.getDistanceFromOrigin() < 0.001f)
        {
            position = targetPosition;
            velocity = juce::Point<float>(0.0f, 0.0f);
        }
    }
    
    repaint();
}

void XYControlComponent::setPosition(juce::Point<float> pos)
{
    position = pos;
    targetPosition = pos;
    velocity = juce::Point<float>(0.0f, 0.0f);
    repaint();
}

void XYControlComponent::setColorScheme(int scheme)
{
    currentColorScheme = juce::jlimit(0, 2, scheme);
    repaint();
}

void XYControlComponent::startHoldTimer()
{
    isHolding = true;
    holdProgress = 0.0f;
    holdStartTime = juce::Time::currentTimeMillis();
}

void XYControlComponent::stopHoldTimer()
{
    isHolding = false;
    holdProgress = 0.0f;
}

void XYControlComponent::triggerDisperse()
{
    isDispersing = true;
    disperseProgress = 0.0f;
    disperseParticles.clear();
    disperseVelocities.clear();
    
    // Create particles in a circle around the current position
    int numParticles = 12;
    juce::Random random;
    
    for (int i = 0; i < numParticles; ++i)
    {
        float angle = (i / (float)numParticles) * juce::MathConstants<float>::twoPi;
        disperseParticles.add(position);
        
        float speed = 2.0f + random.nextFloat() * 1.0f;
        juce::Point<float> vel(std::cos(angle) * speed, std::sin(angle) * speed);
        disperseVelocities.add(vel);
    }
}

void XYControlComponent::cycleColorScheme()
{
    currentColorScheme = (currentColorScheme + 1) % 3;
}
