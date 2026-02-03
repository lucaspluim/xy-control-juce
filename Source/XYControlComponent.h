#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <array>

class XYControlComponent : public juce::Component,
                           private juce::Timer
{
public:
    enum class Preset {
        Blue = 0,
        Red = 1,
        Black = 2
    };

    XYControlComponent();
    ~XYControlComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;

    Preset getCurrentPreset() const { return currentPreset; }
    void setPreset(Preset preset);

private:
    void timerCallback() override;
    void loadGlowImagesFromBinaryData();

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
            float fx = (targetX - x) * stiffness;
            float fy = (targetY - y) * stiffness;

            vx = vx * damping + (fx / mass) * dt;
            vy = vy * damping + (fy / mass) * dt;

            x += vx * dt;
            y += vy * dt;
        }
    };

    std::array<SpringLayer, 6> springLayers;

    float cursorX = 0.5f;
    float cursorY = 0.5f;
    float targetX = 0.5f;
    float targetY = 0.5f;

    bool isDragging = false;
    float breathePhase = 0.0f;
    float breatheBlend = 0.0f;

    Preset currentPreset = Preset::Blue;
    std::array<juce::Image, 5> glowLayers;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYControlComponent)
};
