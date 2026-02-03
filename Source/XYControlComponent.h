#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <array>

class XYControlComponent : public juce::Component,
                           private juce::Timer
{
public:
    XYControlComponent();
    ~XYControlComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    void timerCallback() override;

    struct SpringLayer
    {
        float x, y;
        float vx, vy;
        float stiffness, damping, mass;

        SpringLayer(float stiff, float damp, float m)
            : x(0.5f), y(0.5f), vx(0), vy(0),
              stiffness(stiff), damping(damp), mass(m) {}

        void update(float targetX, float targetY, float dt)
        {
            // Spring force
            float fx = (targetX - x) * stiffness;
            float fy = (targetY - y) * stiffness;

            // Damping force
            float dampingFx = vx * damping;
            float dampingFy = vy * damping;

            // Acceleration
            float ax = (fx - dampingFx) / mass;
            float ay = (fy - dampingFy) / mass;

            // Velocity
            vx += ax * dt;
            vy += ay * dt;

            // Position
            x += vx * dt;
            y += vy * dt;
        }
    };

    struct GlowLayer
    {
        int size;
        float opacity;
        juce::Colour color;
        juce::Image cachedImage;
    };

    std::array<SpringLayer, 6> springLayers;
    std::array<GlowLayer, 5> glowLayers;

    float targetX = 0.5f;
    float targetY = 0.5f;
    bool isDragging = false;
    int64_t lastFrameTime;
    float idleTimer = 0.0f;
    bool isBreathing = true;
    float breatheTime = 0.0f;

    void loadGlowImagesFromBinaryData();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYControlComponent)
};
