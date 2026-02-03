#include "PluginProcessor.h"
#include "PluginEditor.h"

XYControlAudioProcessor::XYControlAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
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
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool XYControlAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool XYControlAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
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

void XYControlAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String XYControlAudioProcessor::getProgramName (int index)
{
    return {};
}

void XYControlAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void XYControlAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void XYControlAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool XYControlAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void XYControlAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
}

bool XYControlAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* XYControlAudioProcessor::createEditor()
{
    return new XYControlAudioProcessorEditor (*this);
}

void XYControlAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(xyPosition.x);
    stream.writeFloat(xyPosition.y);
}

void XYControlAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    xyPosition.x = stream.readFloat();
    xyPosition.y = stream.readFloat();
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new XYControlAudioProcessor();
}
