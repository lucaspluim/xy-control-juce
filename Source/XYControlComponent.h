#pragma once

#include <JuceHeader.h>

class XYControlComponent : public juce::Component
{
public:
    XYControlComponent();
    ~XYControlComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;

    juce::Point<float> getPosition() const { return position; }
    void setPosition(juce::Point<float> pos);
    
    int getColorScheme() const { return currentColorScheme; }
    void setColorScheme(int scheme);

private:
    void updatePhysics();
    void startHoldTimer();
    void stopHoldTimer();
    void triggerDisperse();
    void cycleColorScheme();

    juce::Point<float> position { 0.5f, 0.5f };
    juce::Point<float> velocity { 0.0f, 0.0f };
    juce::Point<float> targetPosition { 0.5f, 0.5f };
    
    bool isDragging = false;
    bool isHolding = false;
    float holdProgress = 0.0f;
    int64 holdStartTime = 0;
    const int64 holdDuration = 3000; // 3 seconds in milliseconds
    
    // Disperse effect
    bool isDispersing = false;
    float disperseProgress = 0.0f;
    juce::Array<juce::Point<float>> disperseParticles;
    juce::Array<juce::Point<float>> disperseVelocities;
    
    // Breathing animation
    float breathingPhase = 0.0f;
    
    // Color schemes: 0 = Blue, 1 = Red, 2 = Black
    int currentColorScheme = 0;
    
    // Double-click detection
    int64 lastClickTime = 0;
    const int64 doubleClickThreshold = 300; // milliseconds
    
    // Animation timer
    class AnimationTimer : public juce::Timer
    {
    public:
        AnimationTimer(XYControlComponent& owner) : owner(owner) {}
        void timerCallback() override { owner.updatePhysics(); }
    private:
        XYControlComponent& owner;
    };
    
    AnimationTimer animationTimer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYControlComponent)
};
