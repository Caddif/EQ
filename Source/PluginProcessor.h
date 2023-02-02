#pragma once

#include "Params.h"
#include <JuceHeader.h>

NoiseGateChain getNoiseGateChainSettings(juce::AudioProcessorValueTreeState& apvts);
CompressorChain getCompressorChainSettings(juce::AudioProcessorValueTreeState& apvts);
EqChain getEqChainSettings(juce::AudioProcessorValueTreeState& apvts);

using NoiseGateIn = juce::dsp::NoiseGate<float>;
using CompressorIn = juce::dsp::Compressor<float>;
using Filter = juce::dsp::IIR::Filter<float>;
using TotalGain = juce::dsp::Gain<float>;
using PassFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
using AudioChain = juce::dsp::ProcessorChain<NoiseGateIn, CompressorIn, PassFilter, Filter, Filter, Filter, PassFilter, TotalGain>;

void updateCoeffs(Filter::CoefficientsPtr& old, const Filter::CoefficientsPtr& newc);

Filter::CoefficientsPtr makeFilter(const EqChain& chainSettings, double sampleRate);
Filter::CoefficientsPtr makeFilter2(const EqChain& chainSettings, double sampleRate);
Filter::CoefficientsPtr makeFilter3(const EqChain& chainSettings, double sampleRate);

inline juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> makeHiPassFilter(const EqChain& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        chainSettings.HiPassFreq,
        sampleRate,
        2 * (chainSettings.HiPassSlope + 1));
}

inline juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> makeLowPassFilter(const EqChain& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        chainSettings.lowPassFreq,
        sampleRate,
        2 * (chainSettings.lowPassSlope + 1));
}

template<typename ChainType, typename CoefficientType>
void updatePassFilter(ChainType& chain, const CoefficientType& cutCoeffs, const Slope& passSlope)
{
    chain.template setBypassed<0>(true);
    chain.template setBypassed<1>(true);
    chain.template setBypassed<2>(true);
    chain.template setBypassed<3>(true);

    switch( passSlope )
    {
        case Slope12:
        {
            *chain.template get<0>().coefficients = *cutCoeffs[0];
            chain.template setBypassed<0>(false);
            break;
        }
        case Slope24:
        {
            *chain.template get<0>().coefficients = *cutCoeffs[0];
            chain.template setBypassed<0>(false);
            *chain.template get<1>().coefficients = *cutCoeffs[1];
            chain.template setBypassed<1>(false);
            break;
        }
        case Slope36:
        {
            *chain.template get<0>().coefficients = *cutCoeffs[0];
            chain.template setBypassed<0>(false);
            *chain.template get<1>().coefficients = *cutCoeffs[1];
            chain.template setBypassed<1>(false);
            *chain.template get<2>().coefficients = *cutCoeffs[2];
            chain.template setBypassed<2>(false);
            break;
        }
        case Slope48:
        {
            *chain.template get<0>().coefficients = *cutCoeffs[0];
            chain.template setBypassed<0>(false);
            *chain.template get<1>().coefficients = *cutCoeffs[1];
            chain.template setBypassed<1>(false);
            *chain.template get<2>().coefficients = *cutCoeffs[2];
            chain.template setBypassed<2>(false);
            *chain.template get<3>().coefficients = *cutCoeffs[3];
            chain.template setBypassed<3>(false);
            break;
        }
    }
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

    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    AudioChain lChain, rChain;

    void bypassedBands(const EqChain& chainSettings, const CompressorChain& compChain, const NoiseGateChain& ngChain);

    void updateGate(const NoiseGateChain& ngChain);
    void updateCompressor(const CompressorChain& compChain);

    void updateFilters(const EqChain& ChainSettings);
    void updateHiPassFilter(const EqChain& chainSettings);
    void updateLowPassFilter(const EqChain& chainSettings);
    void updateTotalGain(const EqChain& chainSettings);

    void updateAllParams();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessor)
};
