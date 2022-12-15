/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin processor.
  ==============================================================================
*/

#pragma once

#include "Params.h"
#include <JuceHeader.h>

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

using TotalGain = juce::dsp::Gain<float>;
using Filter = juce::dsp::IIR::Filter<float>;
using PassFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
using MonoChain = juce::dsp::ProcessorChain<PassFilter, Filter, Filter, Filter, PassFilter, TotalGain>;

void updateCoeffs(Filter::CoefficientsPtr& old, const Filter::CoefficientsPtr& replacements);

Filter::CoefficientsPtr makePeakFilter(const ChainSettings& chainSettings, double sampleRate);
Filter::CoefficientsPtr makePeakFilter2(const ChainSettings& chainSettings, double sampleRate);
Filter::CoefficientsPtr makePeakFilter3(const ChainSettings& chainSettings, double sampleRate);


inline auto makeHiPassFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        chainSettings.HiPassFreq,
        sampleRate,
        2 * (chainSettings.HiPassSlope + 1));
}

inline auto makeLowPassFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        chainSettings.lowPassFreq,
        sampleRate,
        2 * (chainSettings.lowPassSlope + 1));
}
//==============================================================================
/**
*/
class EQAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    EQAudioProcessor();
    ~EQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    MonoChain leftChain, rightChain;

    void bypassHiPass();

    void updatePeakFilters(const ChainSettings& ChainSettings);
    void updateHiPassFilter(const ChainSettings& chainSettings);
    void updateLowPassFilter(const ChainSettings& chainSettings);
    void updateTotalGain(const ChainSettings& chainSettings);
    void updateAllParams();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessor)
};
