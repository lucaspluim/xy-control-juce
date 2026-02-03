#pragma once
#include <juce_core/juce_core.h>
#include <functional>

class NativeDialogs
{
public:
    static void showSaveDialog(const juce::File& presetsFolder, std::function<void(juce::File)> callback);
    static void showConfirmation(const juce::String& title, const juce::String& message, std::function<void()> callback);
    static void showPresetBrowser(const juce::File& presetsFolder,
                                  std::function<void(juce::File)> onLoad);
    static void showPresetOptions(const juce::File& presetsFolder,
                                  std::function<void(juce::File)> onSave,
                                  std::function<void(juce::File)> onLoad);
    static juce::File getPresetsFolder();
    static void createNewFolder(const juce::File& parentFolder, std::function<void(bool)> callback);
};
