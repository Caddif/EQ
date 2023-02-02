#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"

class ResponseCurveComp :
    public juce::Component,
    public juce::AudioProcessorParameter::Listener,
    public juce::Timer
{
public:
    ResponseCurveComp(EQAudioProcessor&);
    ~ResponseCurveComp();

    void parameterValueChanged(int, float);

    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}
    void timerCallback() override;
    void paint(juce::Graphics& g) override;

private:
    EQAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChange{ true };
    AudioChain audiochain;
    std::vector<float> lines;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponseCurveComp)
};