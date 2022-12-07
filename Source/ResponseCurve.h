/*
  ==============================================================================

    ResponseCurve.h
    Created: 17 Nov 2022 3:10:09am
    Author:  Filip Tomeczek

  ==============================================================================
*/

#pragma once

struct ResponseCurveComp :
    juce::Component,
    juce::AudioProcessorParameter::Listener,
    juce::Timer
{
    inline ResponseCurveComp(EQAudioProcessor&);
    inline ~ResponseCurveComp();

    inline void parameterValueChanged(int parameterIndex, float newValue) override;
    inline void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}
    inline void timerCallback() override;
    inline void paint(juce::Graphics& g) override;
private:
    EQAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChange{ true };
    MonoChain monochain;
};

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

        auto lowCutCoeffs = makeHiPassFilter(chainSettings, audioProcessor.getSampleRate());
        auto hiCutCoeffs = makeLowPassFilter(chainSettings, audioProcessor.getSampleRate());

        updateCutFilter(monochain.get<Chainpositons::HiPass>(), lowCutCoeffs, chainSettings.HiPassSlope);
        updateCutFilter(monochain.get<Chainpositons::LowPass>(), hiCutCoeffs, chainSettings.lowPassSlope);

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

    auto& lowCut = monochain.get<Chainpositons::HiPass>();
    auto& peak = monochain.get<Chainpositons::Peak>();
    auto& hiCut = monochain.get<Chainpositons::LowPass>();

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
    g.strokePath(responseCurve, juce::PathStrokeType(1.5f));
}