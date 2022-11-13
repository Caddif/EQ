/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin processor.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum Slope
{
    Slope12,
    Slope24,
    Slope36,
    Slope48
};

struct ChainSettings 
{
    float peakFreq{ 0 }, peakGainInDb{ 0 }, peakQuality{ 1.f };
    float lowCutFreq{ 0 }, hiCutFreq{ 0 };
    Slope lowCutSlope{ Slope::Slope12 }, hiCutSlope{ Slope::Slope12 };
};
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

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

    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

    MonoChain leftChain, rightChain;
    enum Chainpositons
    {
        LowCut,
        Peak,
        HiCut
    };

    void updatePeakFilter(const ChainSettings& ChainSettings);
    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoeffs(Coefficients& old, const Coefficients& replacements);

    template<int Id, typename Chaintype, typename CoefficientType>
    void update(Chaintype& chain, const CoefficientType& coefficeints)
    {
        updateCoeffs(chain.template get<Id>().coefficients, coefficeints[Id]);
        chain.template setBypassed<Id>(false);
    }

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& chain, const CoefficientType& cutCoeffs, const Slope& lowCutSlope)
    {
        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);
        chain.template setBypassed<2>(true);
        chain.template setBypassed<3>(true);

        switch (lowCutSlope)
        {
        case Slope48:
        {
            update<3>(chain, cutCoeffs);
        }
        case Slope36:
        {
            update<2>(chain, cutCoeffs);
        }
        case Slope24:
        {
            update<1>(chain, cutCoeffs);
        }
        case Slope12:
        {
            update<0>(chain, cutCoeffs);
        } 
        }

    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessor)
};
