#include "ResponseGraph.h"

ResponseCurveComp::ResponseCurveComp(EQAudioProcessor& p) : audioProcessor(p)
{
    const auto& params = audioProcessor.getParameters();
    for (auto i : params)
    {
        i->addListener(this);
    }

    startTimerHz(60);

    for (int i = 1; i < 101; i = i * 10)
    {
        lines.push_back(30.f * i);
        lines.push_back(50.f * i);
        lines.push_back(70.f * i);
        lines.push_back(80.f * i);
        lines.push_back(90.f * i);
        lines.push_back(100.f * i);
    }
}

ResponseCurveComp::~ResponseCurveComp()
{
    const auto& params = audioProcessor.getParameters();
    for (auto i : params)
    {
        i->removeListener(this);
    }

    lines.clear();
}

void ResponseCurveComp::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChange.set(true);
}

void ResponseCurveComp::timerCallback()
{
    if (parametersChange.compareAndSetBool(false, true))
    {
        auto chainSettings = getEqChainSettings(audioProcessor.apvts);

        auto peakCoeffs = makeFilter(chainSettings, audioProcessor.getSampleRate());
        auto peakCoeffs2 = makeFilter2(chainSettings, audioProcessor.getSampleRate());
        auto peakCoeffs3 = makeFilter3(chainSettings, audioProcessor.getSampleRate());

        updateCoeffs(audiochain.get<Chainpositons::Filter1>().coefficients, peakCoeffs);
        updateCoeffs(audiochain.get<Chainpositons::Filter2>().coefficients, peakCoeffs2);
        updateCoeffs(audiochain.get<Chainpositons::Filter3>().coefficients, peakCoeffs3);

        auto hiPassCoeffs = makeHiPassFilter(chainSettings, audioProcessor.getSampleRate());
        auto lowPassCoeffs = makeLowPassFilter(chainSettings, audioProcessor.getSampleRate());

        updatePassFilter(audiochain.get<Chainpositons::HiPass>(), hiPassCoeffs, chainSettings.HiPassSlope);
        updatePassFilter(audiochain.get<Chainpositons::LowPass>(), lowPassCoeffs, chainSettings.lowPassSlope);

        audiochain.get<Chainpositons::Gain>().setGainDecibels(chainSettings.totalGain);

        audiochain.setBypassed<Chainpositons::HiPass>(chainSettings.hfpBypass);
        audiochain.setBypassed<Chainpositons::Filter1>(chainSettings.filter1bypass);
        audiochain.setBypassed<Chainpositons::Filter2>(chainSettings.filter2bypass);
        audiochain.setBypassed<Chainpositons::Filter3>(chainSettings.filter3bypass);
        audiochain.setBypassed<Chainpositons::LowPass>(chainSettings.lpfBypass);

        repaint();
    }
}

void ResponseCurveComp::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::darkgrey.withBrightness(0.15f));
    g.setColour(juce::Colours::white.withAlpha(0.07f));

    for (auto i : lines)
    {
        g.drawVerticalLine(static_cast<int>((log10(i / 20.f) / 3) * getLocalBounds().getWidth()), 0, getLocalBounds().getHeight());
    }

    auto &hiPass = audiochain.get<Chainpositons::HiPass>();
    auto &peak = audiochain.get<Chainpositons::Filter1>();
    auto &peak2 = audiochain.get<Chainpositons::Filter2>();
    auto &peak3 = audiochain.get<Chainpositons::Filter3>();
    auto &lowPass = audiochain.get<Chainpositons::LowPass>();
    auto &totalGain = audiochain.get<Chainpositons::Gain>();

    auto sampleRate = audioProcessor.getSampleRate();
    std::vector<double> magnitudes;
    magnitudes.resize(getLocalBounds().getWidth());

    juce::Path responseCurve;

    for (int i = 0; i < getLocalBounds().getWidth(); ++i)
    {
        double mag = 1.f;
        auto freq = juce::mapToLog10(double(i) / static_cast<double>(getLocalBounds().getWidth()), 20.0, 20000.0);

        if (!audiochain.isBypassed<HiPass>())
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

        if (!audiochain.isBypassed<Filter1>())
            if (!audiochain.isBypassed<Chainpositons::Filter1>())
                mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!audiochain.isBypassed<Filter2>())
            if (!audiochain.isBypassed<Chainpositons::Filter2>())
                mag *= peak2.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!audiochain.isBypassed<Filter3>())
            if (!audiochain.isBypassed<Chainpositons::Filter3>())
                mag *= peak3.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!audiochain.isBypassed<LowPass>())
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

    responseCurve.startNewSubPath(getLocalBounds().getX(),
        juce::jmap(magnitudes.front(), -24.0, 24.0, static_cast<double>(getLocalBounds().getHeight()), static_cast<double>(getLocalBounds().getY())));

    for (size_t i = 0; i < magnitudes.size(); ++i)
    {
        responseCurve.lineTo(getLocalBounds().getX() + i,
            juce::jmap(magnitudes[i], -24.0, 24.0, static_cast<double>(getLocalBounds().getHeight()), static_cast<double>(getLocalBounds().getY())));
    }

    g.setColour(juce::Colours::white);
    g.strokePath(responseCurve, juce::PathStrokeType(1.5f));
}