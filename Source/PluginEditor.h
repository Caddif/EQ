/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/
#pragma once

#include "Params.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomSlider : juce::Slider
{
    CustomSlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow) {}
};

struct ResponseCurveComp :
    juce::Component,
    juce::AudioProcessorParameter::Listener,
    juce::Timer
{
    ResponseCurveComp(EQAudioProcessor&);
    ~ResponseCurveComp();

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}
    void timerCallback() override;
    void paint(juce::Graphics& g) override;
private:
    EQAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChange{ false };
    MonoChain monochain;
};

class EQAudioProcessorEditor  : 
    public juce::AudioProcessorEditor
{
public:
    EQAudioProcessorEditor (EQAudioProcessor&);
    ~EQAudioProcessorEditor() override;

    //==============================================================================
    //void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EQAudioProcessor& audioProcessor;

    CustomSlider
        peakFreqSlider,
        peakGainSlider, 
        peakQualSlider, 
        lowCutFreqSlider, 
        hiCutFreqSlider, 
        lowCutSlopeSlider, 
        hiCutSlopeSlider;

    juce::AudioProcessorValueTreeState::SliderAttachment
        peakFreqSliderAtt,
        peakGainSliderAtt,
        peakQualSliderAtt,
        lowCutFreqSliderAtt,
        hiCutFreqSliderAtt,
        lowCutSlopeSliderAtt,
        hiCutSlopeSliderAtt;
    
    ResponseCurveComp responseCurveComp;
    std::vector<juce::Component*> getComponents();

    //MonoChain monochian;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessorEditor)
};
