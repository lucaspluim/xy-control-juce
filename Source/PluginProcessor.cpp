#include "PluginProcessor.h"
#include "PluginEditor.h"

XYControlAudioProcessor::XYControlAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

XYControlAudioProcessor::~XYControlAudioProcessor()
{
}

const juce::String XYControlAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool XYControlAudioProcessor::acceptsMidi() const
{
    return false;
}

bool XYControlAudioProcessor::producesMidi() const
{
    return false;
}

bool XYControlAudioProcessor::isMidiEffect() const
{
    return false;
}

double XYControlAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int XYControlAudioProcessor::getNumPrograms()
{
    return 1;
}

int XYControlAudioProcessor::getCurrentProgram()
{
    return 0;
}

void XYControlAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String XYControlAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void XYControlAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void XYControlAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void XYControlAudioProcessor::releaseResources()
{
}

bool XYControlAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void XYControlAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    // Pass-through audio
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
}

bool XYControlAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* XYControlAudioProcessor::createEditor()
{
    return new XYControlAudioProcessorEditor(*this);
}

void XYControlAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ignoreUnused(destData);
}

void XYControlAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new XYControlAudioProcessor();
}
