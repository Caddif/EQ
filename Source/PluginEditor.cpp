/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"

ResponseCurveComp::ResponseCurveComp(EQAudioProcessor& p) : audioProcessor(p)
{
    const auto& params = audioProcessor.getParameters();
    for (auto i : params)
    {
        i->addListener(this);
    }

    startTimerHz(60);
}

ResponseCurveComp::~ResponseCurveComp()
{
    const auto& params = audioProcessor.getParameters();
    for (auto i : params)
    {
        i->removeListener(this);
    }
}

void ResponseCurveComp::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChange.set(true);
}

void ResponseCurveComp::timerCallback()
{
    if (parametersChange.compareAndSetBool(false, true))
    {
        auto chainSettings = getChainSettings(audioProcessor.apvts);
        auto peakCoeffs = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
        updateCoeffs(monochain.get<Chainpositons::Peak>().coefficients, peakCoeffs);

        auto lowCutCoeffs = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
        auto hiCutCoeffs = makeHiCutFilter(chainSettings, audioProcessor.getSampleRate());

        updateCutFilter(monochain.get<Chainpositons::LowCut>(), lowCutCoeffs, chainSettings.lowCutSlope);
        updateCutFilter(monochain.get<Chainpositons::HiCut>(), hiCutCoeffs, chainSettings.hiCutSlope);

        repaint();
    }
}

void ResponseCurveComp::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);

    //auto bounds = getLocalBounds();
    auto responseGraphArea = getLocalBounds(); //bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.5));
    int graphWidth = responseGraphArea.getWidth();

    auto& lowCut = monochain.get<Chainpositons::LowCut>();
    auto& peak = monochain.get<Chainpositons::Peak>();
    auto& hiCut = monochain.get<Chainpositons::HiCut>();

    auto sampleRate = audioProcessor.getSampleRate();
    std::vector<double> magnitudes;
    magnitudes.resize(graphWidth);

    for (int i = 0; i < graphWidth; ++i)
    {
        double mag = 1.f;
        auto freq = juce::mapToLog10(double(i) / double(graphWidth), 20.0, 20000.0);

        if (!monochain.isBypassed<Chainpositons::Peak>())
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!lowCut.isBypassed<0>())
            mag *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<1>())
            mag *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<2>())
            mag *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowCut.isBypassed<3>())
            mag *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!hiCut.isBypassed<0>())
            mag *= hiCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!hiCut.isBypassed<1>())
            mag *= hiCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!hiCut.isBypassed<2>())
            mag *= hiCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!hiCut.isBypassed<3>())
            mag *= hiCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        magnitudes[i] = juce::Decibels::gainToDecibels(mag);
    }
    juce::Path responseCurve;
    const double maxOut = responseGraphArea.getY();
    const double minOut = responseGraphArea.getBottom();

    auto map = [maxOut, minOut](double input)
    {
        return juce::jmap(input, -24.0, 24.0, minOut, maxOut);
    };

    responseCurve.startNewSubPath(responseGraphArea.getX(), map(magnitudes.front()));

    for (size_t i = 0; i < magnitudes.size(); ++i)
    {
        responseCurve.lineTo(responseGraphArea.getX() + i, map(magnitudes[i]));
    }

    g.setColour(juce::Colours::red);
    g.drawRoundedRectangle(responseGraphArea.toFloat(), 5.f, 1.f);

    g.setColour(juce::Colours::white);
    g.strokePath(responseCurve, juce::PathStrokeType(2.f));
}

// Beneth EQAudioProcessorEditor 
//==============================================================================
EQAudioProcessorEditor::EQAudioProcessorEditor (EQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    responseCurveComp(audioProcessor),
    peakFreqSliderAtt( audioProcessor.apvts, "PeakFreq", peakFreqSlider ),
    peakGainSliderAtt( audioProcessor.apvts, "PeakGain", peakGainSlider ),
    peakQualSliderAtt( audioProcessor.apvts, "PeakQual", peakQualSlider ),
    lowCutFreqSliderAtt( audioProcessor.apvts, "LowCutFreq", lowCutFreqSlider ),
    hiCutFreqSliderAtt( audioProcessor.apvts, "HiCutFreq", hiCutFreqSlider ),
    lowCutSlopeSliderAtt( audioProcessor.apvts, "LowCutSlope", lowCutSlopeSlider ),
    hiCutSlopeSliderAtt( audioProcessor.apvts, "HiCutSlope", hiCutSlopeSlider )
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    for (auto* components : getComponents())
    {
        addAndMakeVisible(components);
    }

    setSize (1000, 400);
}

EQAudioProcessorEditor::~EQAudioProcessorEditor(){}
//==============================================================================

void EQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto responseGraphArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.5));
    auto lowCutArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.33));
    auto hiCutArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.50));

    responseCurveComp.setBounds(responseGraphArea);

    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(static_cast<int>(bounds.getHeight() * 0.50)));
    lowCutSlopeSlider.setBounds(lowCutArea);
    hiCutFreqSlider.setBounds(hiCutArea.removeFromTop(static_cast<int>(bounds.getHeight() * 0.50)));
    hiCutSlopeSlider.setBounds(hiCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.33)));
    peakGainSlider.setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.50)));
    peakQualSlider.setBounds(bounds);

}

std::vector<juce::Component*> EQAudioProcessorEditor::getComponents()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualSlider,
        &lowCutFreqSlider,
        &hiCutFreqSlider,
        &lowCutSlopeSlider,
        &hiCutSlopeSlider,
        &responseCurveComp
    };
}
