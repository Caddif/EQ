#include "ResponseGraph.h"

ResponseCurveComp::ResponseCurveComp(EQAudioProcessor& p) : audioProcessor(p)
{
    const auto& params = audioProcessor.getParameters();
    for (auto i : params)
    {
        i->addListener(this);
    }

    startTimerHz(144);
    juce::Time::waitForMillisecondCounter(10);
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

        auto peakCoeffs = makeFilter(chainSettings, audioProcessor.getSampleRate());
        auto peakCoeffs2 = makeFilter2(chainSettings, audioProcessor.getSampleRate());
        auto peakCoeffs3 = makeFilter3(chainSettings, audioProcessor.getSampleRate());

        updateCoeffs(monochain.get<Chainpositons::Filter1>().coefficients, peakCoeffs);
        updateCoeffs(monochain.get<Chainpositons::Filter2>().coefficients, peakCoeffs2);
        updateCoeffs(monochain.get<Chainpositons::Filter3>().coefficients, peakCoeffs3);

        auto hiPassCoeffs = makeHiPassFilter(chainSettings, audioProcessor.getSampleRate());
        auto lowPassCoeffs = makeLowPassFilter(chainSettings, audioProcessor.getSampleRate());

        updatePassFilter(monochain.get<Chainpositons::HiPass>(), hiPassCoeffs, chainSettings.HiPassSlope);
        updatePassFilter(monochain.get<Chainpositons::LowPass>(), lowPassCoeffs, chainSettings.lowPassSlope);

        monochain.get<Chainpositons::Gain>().setGainDecibels(chainSettings.totalGain);

        monochain.setBypassed<Chainpositons::HiPass>(chainSettings.hfpBypass);
        monochain.setBypassed<Chainpositons::Filter1>(chainSettings.peak1Bypass);
        monochain.setBypassed<Chainpositons::Filter2>(chainSettings.peak2Bypass);
        monochain.setBypassed<Chainpositons::Filter3>(chainSettings.peak3Bypass);
        monochain.setBypassed<Chainpositons::LowPass>(chainSettings.lpfBypass);

        repaint();
    }
}

void ResponseCurveComp::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::darkgrey.withBrightness(0.15f));

    auto responseGraphArea = getLocalBounds(); 

    int graphWidth = responseGraphArea.getWidth();

    auto& hiPass = monochain.get<Chainpositons::HiPass>();
    auto& peak = monochain.get<Chainpositons::Filter1>();
    auto& peak2 = monochain.get<Chainpositons::Filter2>();
    auto& peak3 = monochain.get<Chainpositons::Filter3>();
    auto& lowPass = monochain.get<Chainpositons::LowPass>();
    auto& totalGain = monochain.get<Chainpositons::Gain>();

    auto sampleRate = audioProcessor.getSampleRate();
    std::vector<double> magnitudes;
    magnitudes.resize(graphWidth);

    for (int i = 0; i < graphWidth; ++i)
    {
        double mag = 1.f;
        auto freq = juce::mapToLog10(double(i) / double(graphWidth), 20.0, 20000.0);

        if (!monochain.isBypassed<HiPass>())
        {
            if (!hiPass.isBypassed<0>())
                mag *= hiPass.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!hiPass.isBypassed<1>())
                mag *= hiPass.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!hiPass.isBypassed<2>())
                mag *= hiPass.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!hiPass.isBypassed<3>())
                mag *= hiPass.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }


        if (!monochain.isBypassed<Filter1>())
            if (!monochain.isBypassed<Chainpositons::Filter1>())
                mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!monochain.isBypassed<Filter2>())
            if (!monochain.isBypassed<Chainpositons::Filter2>())
                mag *= peak2.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!monochain.isBypassed<Filter3>())
            if (!monochain.isBypassed<Chainpositons::Filter3>())
                mag *= peak3.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!monochain.isBypassed<LowPass>()) 
        {
            if (!lowPass.isBypassed<0>())
                mag *= lowPass.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!lowPass.isBypassed<1>())
                mag *= lowPass.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!lowPass.isBypassed<2>())
                mag *= lowPass.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!lowPass.isBypassed<3>())
                mag *= lowPass.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }

        magnitudes[i] = juce::Decibels::gainToDecibels(mag) + totalGain.getGainDecibels();
    }

    juce::Path responseCurve;
    const double maxOut = responseGraphArea.getY();
    const double minOut = responseGraphArea.getHeight();

    auto map = [maxOut, minOut](double input)
    {
        return juce::jmap(input, -24.0, 24.0, minOut, maxOut);
    };

    responseCurve.startNewSubPath(responseGraphArea.getX(), map(magnitudes.front()));

    for (size_t i = 0; i < magnitudes.size(); ++i)
    {
        responseCurve.lineTo(responseGraphArea.getX() + i, map(magnitudes[i]));
    }

    g.setColour(juce::Colours::white);
    g.strokePath(responseCurve, juce::PathStrokeType(1.5f));
}