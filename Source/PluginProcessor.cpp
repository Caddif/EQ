/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin processor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EQAudioProcessor::EQAudioProcessor()
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

EQAudioProcessor::~EQAudioProcessor()
{
}

//==============================================================================
const juce::String EQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EQAudioProcessor::getProgramName (int index)
{
    return {};
}

void EQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    leftChain.get<Chainpositons::Gain>().setRampDurationSeconds(0.02);
    rightChain.get<Chainpositons::Gain>().setRampDurationSeconds(0.02);

    updateAllfilterParams();
}

void EQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void EQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // You must update parameters before processing audio
    updateAllfilterParams();

    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

}

//==============================================================================
bool EQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EQAudioProcessor::createEditor()
{
    return new EQAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void EQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream memory_out_stream(destData, true);
    apvts.state.writeToStream(memory_out_stream);

}

void EQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto valTree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (valTree.isValid())
    {
        apvts.replaceState(valTree);
        updateAllfilterParams();
    }

}

void EQAudioProcessor::bypassLowCut()
{
    // just testing
    leftChain.setBypassed<HiPass>(true);
    rightChain.setBypassed<HiPass>(true);
}

void EQAudioProcessor::updateTotalGain(const ChainSettings& chainSettings)
{
    leftChain.get<Chainpositons::Gain>().setGainDecibels(chainSettings.totalGain);
    rightChain.get<Chainpositons::Gain>().setGainDecibels(chainSettings.totalGain);
}

void EQAudioProcessor::updatePeakFilters(const ChainSettings& chainSettings)
{
    auto peakCoefficients = makePeakFilter(chainSettings, getSampleRate());
    auto peakCoefficients2 = makePeakFilter2(chainSettings, getSampleRate());
    auto peakCoefficients3 = makePeakFilter3(chainSettings, getSampleRate());

    updateCoeffs(leftChain.get<Chainpositons::Peak>().coefficients, peakCoefficients);
    updateCoeffs(rightChain.get<Chainpositons::Peak>().coefficients, peakCoefficients);
    updateCoeffs(leftChain.get<Chainpositons::Peak2>().coefficients, peakCoefficients2);
    updateCoeffs(rightChain.get<Chainpositons::Peak2>().coefficients, peakCoefficients2);
    updateCoeffs(leftChain.get<Chainpositons::Peak3>().coefficients, peakCoefficients3);
    updateCoeffs(rightChain.get<Chainpositons::Peak3>().coefficients, peakCoefficients3);
}

void updateCoeffs(Filter::CoefficientsPtr & old, const Filter::CoefficientsPtr & replacements)
{
    *old = *replacements;
}

void EQAudioProcessor::updateLowCutFilter(const ChainSettings& chainSettings)
{
    auto hiPassCoeffs = makeHiPassFilter(chainSettings, getSampleRate());
    //bypassLowCut();

    auto& leftLowCut = leftChain.get<Chainpositons::HiPass>();
    auto& rightLowCut = rightChain.get<Chainpositons::HiPass>();

    updateCutFilter(leftLowCut, hiPassCoeffs, chainSettings.HiPassSlope);
    updateCutFilter(rightLowCut, hiPassCoeffs, chainSettings.HiPassSlope);
}

void EQAudioProcessor::updateHiCutFilter(const ChainSettings& chainSettings)
{
    auto lowPassCoeffs = makeLowPassFilter(chainSettings, getSampleRate());

    auto& leftHiCut = leftChain.get<Chainpositons::LowPass>();
    auto& rightHiCut = rightChain.get<Chainpositons::LowPass>();

    updateCutFilter(leftHiCut, lowPassCoeffs, chainSettings.lowPassSlope);
    updateCutFilter(rightHiCut, lowPassCoeffs, chainSettings.lowPassSlope);
}

void EQAudioProcessor::updateAllfilterParams()
{
    auto chainSettings = getChainSettings(apvts);

    updateLowCutFilter(chainSettings);
    updatePeakFilters(chainSettings);
    updateHiCutFilter(chainSettings);
    updateTotalGain(chainSettings);
}

juce::AudioProcessorValueTreeState::ParameterLayout EQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    juce::StringArray stringArr;
    for (int i = 0; i < 4; ++i) {
        juce::String str;
        str << (12 + 12 * i);
        str << "db/Octave";
        stringArr.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>("HiPassSlope", "HiPassSlope", stringArr, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LowPassSlope", "LowPassSlope", stringArr, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HiPassFreq", 
                                                           "HiPassFreq", 
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3f),
                                                           20.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LowPassFreq", 
                                                           "LowPassFreq", 
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3f),
                                                           20000.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq", 
                                                           "PeakFreq", 
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3f),
                                                           700.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq2",
                                                           "PeakFreq2",
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3f),
                                                           1700.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq3",
                                                           "PeakFreq3",
                                                           juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3f),
                                                           5000.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain", 
                                                           "PeakGain", 
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
                                                           0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain2",
                                                           "PeakGain2",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
                                                           0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain3",
                                                           "PeakGain3",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
                                                           0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQual", 
                                                           "PeakQual", 
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                           1.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQual2",
                                                           "PeakQual2",
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                           1.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQual3",
                                                           "PeakQual3",
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                           1.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("TotalGain",
                                                           "TotalGain",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
                                                           0.f));

    return layout;
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;

    settings.HiPassFreq = apvts.getRawParameterValue("HiPassFreq")->load();
    settings.lowPassFreq = apvts.getRawParameterValue("LowPassFreq")->load();
    settings.peakFreq = apvts.getRawParameterValue("PeakFreq")->load();
    settings.peakFreq2 = apvts.getRawParameterValue("PeakFreq2")->load();
    settings.peakFreq3 = apvts.getRawParameterValue("PeakFreq3")->load();
    settings.peakGainInDb = apvts.getRawParameterValue("PeakGain")->load();
    settings.peakGainInDb2 = apvts.getRawParameterValue("PeakGain2")->load();
    settings.peakGainInDb3 = apvts.getRawParameterValue("PeakGain3")->load();
    settings.peakQuality = apvts.getRawParameterValue("PeakQual")->load();
    settings.peakQuality2 = apvts.getRawParameterValue("PeakQual2")->load();
    settings.peakQuality3 = apvts.getRawParameterValue("PeakQual3")->load();
    settings.HiPassSlope = static_cast<Slope>(apvts.getRawParameterValue("HiPassSlope")->load());
    settings.lowPassSlope = static_cast<Slope>(apvts.getRawParameterValue("LowPassSlope")->load());
    settings.totalGain = apvts.getRawParameterValue("TotalGain")->load();

    return settings;
}

Filter::CoefficientsPtr makePeakFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate,
        chainSettings.peakFreq,
        chainSettings.peakQuality,
        juce::Decibels::decibelsToGain(chainSettings.peakGainInDb));
}

Filter::CoefficientsPtr makePeakFilter2(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate,
        chainSettings.peakFreq2,
        chainSettings.peakQuality2,
        juce::Decibels::decibelsToGain(chainSettings.peakGainInDb2));
}

Filter::CoefficientsPtr makePeakFilter3(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate,
        chainSettings.peakFreq3,
        chainSettings.peakQuality3,
        juce::Decibels::decibelsToGain(chainSettings.peakGainInDb3));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQAudioProcessor();
}
