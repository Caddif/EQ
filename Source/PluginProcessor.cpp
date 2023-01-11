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
    juce::dsp::ProcessSpec spec{ sampleRate, samplesPerBlock, 1 };

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    leftChain.get<Chainpositons::Gain>().setRampDurationSeconds(0.02);
    rightChain.get<Chainpositons::Gain>().setRampDurationSeconds(0.02);

    updateAllParams();
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
    updateAllParams();

    juce::dsp::AudioBlock<float> block(buffer);

    // 0 represents left and 1 right channel
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
        updateAllParams();
    }
}

void EQAudioProcessor::bypassedBands(const EqChain& chainSettings)
{
    leftChain.setBypassed<HiPass>(chainSettings.hfpBypass);
    rightChain.setBypassed<HiPass>(chainSettings.hfpBypass);

    leftChain.setBypassed<Filter1>(chainSettings.peak1Bypass);
    rightChain.setBypassed<Filter1>(chainSettings.peak1Bypass);

    leftChain.setBypassed<Filter2>(chainSettings.peak2Bypass);
    rightChain.setBypassed<Filter2>(chainSettings.peak2Bypass);

    leftChain.setBypassed<Filter3>(chainSettings.peak3Bypass);
    rightChain.setBypassed<Filter3>(chainSettings.peak3Bypass);

    leftChain.setBypassed<LowPass>(chainSettings.lpfBypass);
    rightChain.setBypassed<LowPass>(chainSettings.lpfBypass);
}

void EQAudioProcessor::updateGate(const NoiseGateChain& ngChain)
{ 
    leftChain.get<Chainpositons::NoiseGate>().setThreshold(ngChain.threshold);
    rightChain.get<Chainpositons::NoiseGate>().setThreshold(ngChain.threshold);

    leftChain.get<Chainpositons::NoiseGate>().setRatio(ngChain.ratio);
    rightChain.get<Chainpositons::NoiseGate>().setRatio(ngChain.ratio);

    leftChain.get<Chainpositons::NoiseGate>().setAttack(ngChain.attack);
    rightChain.get<Chainpositons::NoiseGate>().setAttack(ngChain.attack);

    leftChain.get<Chainpositons::NoiseGate>().setRelease(ngChain.release);
    rightChain.get<Chainpositons::NoiseGate>().setRelease(ngChain.release);
}

void EQAudioProcessor::updateCompressor(const CompressorChain& compChain)
{
    leftChain.get<Chainpositons::Compressor>().setThreshold(compChain.threshold);
    rightChain.get<Chainpositons::Compressor>().setThreshold(compChain.threshold);

    leftChain.get<Chainpositons::Compressor>().setRatio(compChain.ratio);
    rightChain.get<Chainpositons::Compressor>().setRatio(compChain.ratio);

    leftChain.get<Chainpositons::Compressor>().setAttack(compChain.attack);
    rightChain.get<Chainpositons::Compressor>().setAttack(compChain.attack);

    leftChain.get<Chainpositons::Compressor>().setRelease(compChain.release);
    rightChain.get<Chainpositons::Compressor>().setRelease(compChain.release);
}

void EQAudioProcessor::updateTotalGain(const EqChain& chainSettings)
{
    leftChain.get<Chainpositons::Gain>().setGainDecibels(chainSettings.totalGain);
    rightChain.get<Chainpositons::Gain>().setGainDecibels(chainSettings.totalGain);
}

void EQAudioProcessor::updateFilters(const EqChain& chainSettings)
{
    auto Coefficients = makeFilter(chainSettings, getSampleRate());
    auto Coefficients2 = makeFilter2(chainSettings, getSampleRate());
    auto Coefficients3 = makeFilter3(chainSettings, getSampleRate());

    updateCoeffs(leftChain.get<Chainpositons::Filter1>().coefficients, Coefficients);
    updateCoeffs(rightChain.get<Chainpositons::Filter1>().coefficients, Coefficients);
    updateCoeffs(leftChain.get<Chainpositons::Filter2>().coefficients, Coefficients2);
    updateCoeffs(rightChain.get<Chainpositons::Filter2>().coefficients, Coefficients2);
    updateCoeffs(leftChain.get<Chainpositons::Filter3>().coefficients, Coefficients3);
    updateCoeffs(rightChain.get<Chainpositons::Filter3>().coefficients, Coefficients3);
}

void updateCoeffs(Filter::CoefficientsPtr& old, const Filter::CoefficientsPtr& replacements)
{
    *old = *replacements;
}

void EQAudioProcessor::updateHiPassFilter(const EqChain& chainSettings)
{
    auto hiPassCoeffs = makeHiPassFilter(chainSettings, getSampleRate());

    auto& leftHiPass = leftChain.get<Chainpositons::HiPass>();
    auto& rightHiPass = rightChain.get<Chainpositons::HiPass>();

    updatePassFilter(leftHiPass, hiPassCoeffs, chainSettings.HiPassSlope);
    updatePassFilter(rightHiPass, hiPassCoeffs, chainSettings.HiPassSlope);
}

void EQAudioProcessor::updateLowPassFilter(const EqChain& chainSettings)
{
    auto lowPassCoeffs = makeLowPassFilter(chainSettings, getSampleRate());

    auto& leftLowPass = leftChain.get<Chainpositons::LowPass>();
    auto& rightLowPass = rightChain.get<Chainpositons::LowPass>();

    updatePassFilter(leftLowPass, lowPassCoeffs, chainSettings.lowPassSlope);
    updatePassFilter(rightLowPass, lowPassCoeffs, chainSettings.lowPassSlope);
}

void EQAudioProcessor::updateAllParams()
{
    auto chainSettings = getChainSettings(apvts);
    auto noiseGateChain = getNoiseGateChainSettings(apvts);
    auto compressorChain = getCompressorChainSettings(apvts);

    updateGate(noiseGateChain);
    updateCompressor(compressorChain);

    updateHiPassFilter(chainSettings);
    updateFilters(chainSettings);
    updateLowPassFilter(chainSettings);
    
    updateTotalGain(chainSettings);
    bypassedBands(chainSettings);
}

juce::AudioProcessorValueTreeState::ParameterLayout EQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    juce::StringArray stringArrSlope, filterToChoose;
    juce::String str;

    for (int i = 0; i < 4; ++i) {
        str << (12 + 12 * i);
        str << "db/Octave";
        stringArrSlope.add(str);
        str.clear();
    }

    filterToChoose.add("Peakfilter");
    filterToChoose.add("LowShelffilter");
    filterToChoose.add("HighShelffilter");
    filterToChoose.add("Notchfilter");

    layout.add(std::make_unique<juce::AudioParameterChoice>("HiPassSlope", "HiPassSlope", stringArrSlope, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LowPassSlope", "LowPassSlope", stringArrSlope, 0));

    layout.add(std::make_unique<juce::AudioParameterChoice>("Filter1choice", "Filter1choice", filterToChoose, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Filter2choice", "Filter2choice", filterToChoose, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Filter3choice", "Filter3choice", filterToChoose, 0));

    layout.add(std::make_unique<juce::AudioParameterBool>("HPFbypass", "HPFbypass", true));
    layout.add(std::make_unique<juce::AudioParameterBool>("Peak1bypass", "Peak1bypass", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("Peak2bypass", "Peak2bypass", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("Peak3bypass", "Peak3bypass", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("LPFbypass", "LPFbypass", true));

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

    //  Noise gate and Compressor ///////////////////////////////////////////////////////////////////////////////////////////////////
    layout.add(std::make_unique<juce::AudioParameterFloat>("ngThreshold",
        "ngThreshold",
        juce::NormalisableRange<float>(-92.f, 0.f, 0.5f, 1.f),
        -48.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("ngRatio",
        "ngRatio",
        juce::NormalisableRange<float>(1.f, 100.f, 1.f, 0.6f),
        1.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("ngAttack",
        "ngAttack",
        juce::NormalisableRange<float>(0.f, 150.f, 0.5f, 1.f),
        0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("ngRelease",
        "ngRelease",
        juce::NormalisableRange<float>(0.f, 150.f, 0.5f, 1.f),
        0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("compThreshold",
        "compThreshold",
        juce::NormalisableRange<float>(-60.f, 0.f, 1.f, 1.f),
        0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("compRatio",
        "compRatio",
        juce::NormalisableRange<float>(1.f, 40.f, 0.25f, 0.6f),
        1.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("compAttack",
        "compAttack",
        juce::NormalisableRange<float>(0.f, 150.f, 0.5f, 1.f),
        0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("compRelease",
        "compRelease",
        juce::NormalisableRange<float>(0.f, 150.f, 0.5f, 1.f),
        0.f));

    return layout;
}

EqChain getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    EqChain settings;

    settings.HiPassFreq = apvts.getRawParameterValue("HiPassFreq")->load();
    settings.lowPassFreq = apvts.getRawParameterValue("LowPassFreq")->load();
    settings.filterFreq = apvts.getRawParameterValue("PeakFreq")->load();
    settings.filterFreq2 = apvts.getRawParameterValue("PeakFreq2")->load();
    settings.filterFreq3 = apvts.getRawParameterValue("PeakFreq3")->load();
    settings.gainInDb = apvts.getRawParameterValue("PeakGain")->load();
    settings.gainInDb2 = apvts.getRawParameterValue("PeakGain2")->load();
    settings.gainInDb3 = apvts.getRawParameterValue("PeakGain3")->load();
    settings.filterQuality = apvts.getRawParameterValue("PeakQual")->load();
    settings.filterQuality2 = apvts.getRawParameterValue("PeakQual2")->load();
    settings.filterQuality3 = apvts.getRawParameterValue("PeakQual3")->load();
    settings.HiPassSlope = static_cast<Slope>(apvts.getRawParameterValue("HiPassSlope")->load());
    settings.lowPassSlope = static_cast<Slope>(apvts.getRawParameterValue("LowPassSlope")->load());
    settings.totalGain = apvts.getRawParameterValue("TotalGain")->load();
    settings.hfpBypass = apvts.getRawParameterValue("HPFbypass")->load();
    settings.peak1Bypass = apvts.getRawParameterValue("Peak1bypass")->load();
    settings.peak2Bypass = apvts.getRawParameterValue("Peak2bypass")->load();
    settings.peak3Bypass = apvts.getRawParameterValue("Peak3bypass")->load();
    settings.lpfBypass = apvts.getRawParameterValue("LPFbypass")->load();
    settings.Filter1choice = static_cast<FilterToChoose>(apvts.getRawParameterValue("Filter1choice")->load());
    settings.Filter2choice = static_cast<FilterToChoose>(apvts.getRawParameterValue("Filter2choice")->load());
    settings.Filter3choice = static_cast<FilterToChoose>(apvts.getRawParameterValue("Filter3choice")->load());

    return settings;
}

NoiseGateChain getNoiseGateChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    NoiseGateChain settings;

    settings.threshold = apvts.getRawParameterValue("ngThreshold")->load();
    settings.ratio = apvts.getRawParameterValue("ngRatio")->load();
    settings.attack = apvts.getRawParameterValue("ngAttack")->load();
    settings.release = apvts.getRawParameterValue("ngRelease")->load();

    return settings;
}

CompressorChain getCompressorChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    CompressorChain settings;

    settings.threshold = apvts.getRawParameterValue("compThreshold")->load();
    settings.ratio = apvts.getRawParameterValue("compRatio")->load();
    settings.attack = apvts.getRawParameterValue("compAttack")->load();
    settings.release = apvts.getRawParameterValue("compRelease")->load();

    return settings;
}

Filter::CoefficientsPtr makeFilter(const EqChain& chainSettings, double sampleRate)
{
    if(chainSettings.Filter1choice == FilterToChoose::PeakFilter)
        return juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate,
            chainSettings.filterFreq,
            chainSettings.filterQuality,
            juce::Decibels::decibelsToGain(chainSettings.gainInDb));
    else if(chainSettings.Filter1choice == FilterToChoose::LowShelfFilter)
        return juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            sampleRate,
            chainSettings.filterFreq,
            chainSettings.filterQuality,
            juce::Decibels::decibelsToGain(chainSettings.gainInDb));
    else if (chainSettings.Filter1choice == FilterToChoose::HighShelfFilter)
        return juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate,
            chainSettings.filterFreq,
            chainSettings.filterQuality,
            juce::Decibels::decibelsToGain(chainSettings.gainInDb));
    else 
        return juce::dsp::IIR::Coefficients<float>::makeNotch(
            sampleRate,
            chainSettings.filterFreq,
            chainSettings.filterQuality);
}

Filter::CoefficientsPtr makeFilter2(const EqChain& chainSettings, double sampleRate)
{
    if (chainSettings.Filter2choice == FilterToChoose::PeakFilter)
        return juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate,
            chainSettings.filterFreq2,
            chainSettings.filterQuality2,
            juce::Decibels::decibelsToGain(chainSettings.gainInDb2));
    else if (chainSettings.Filter2choice == FilterToChoose::LowShelfFilter)
        return juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            sampleRate,
            chainSettings.filterFreq2,
            chainSettings.filterQuality2,
            juce::Decibels::decibelsToGain(chainSettings.gainInDb2));
    else if (chainSettings.Filter2choice == FilterToChoose::HighShelfFilter)
        return juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate,
            chainSettings.filterFreq2,
            chainSettings.filterQuality2,
            juce::Decibels::decibelsToGain(chainSettings.gainInDb2));
    else
        return juce::dsp::IIR::Coefficients<float>::makeNotch(
            sampleRate,
            chainSettings.filterFreq2,
            chainSettings.filterQuality2);
}

Filter::CoefficientsPtr makeFilter3(const EqChain& chainSettings, double sampleRate)
{
    if (chainSettings.Filter3choice == FilterToChoose::PeakFilter)
        return juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate,
            chainSettings.filterFreq3,
            chainSettings.filterQuality3,
            juce::Decibels::decibelsToGain(chainSettings.gainInDb3));
    else if (chainSettings.Filter3choice == FilterToChoose::LowShelfFilter)
        return juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            sampleRate,
            chainSettings.filterFreq3,
            chainSettings.filterQuality3,
            juce::Decibels::decibelsToGain(chainSettings.gainInDb3));
    else if (chainSettings.Filter3choice == FilterToChoose::HighShelfFilter)
        return juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate,
            chainSettings.filterFreq3,
            chainSettings.filterQuality3,
            juce::Decibels::decibelsToGain(chainSettings.gainInDb3));
    else
        return juce::dsp::IIR::Coefficients<float>::makeNotch(
            sampleRate,
            chainSettings.filterFreq3,
            chainSettings.filterQuality3);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQAudioProcessor();
}
