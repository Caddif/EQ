/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/
#pragma once

#include "Params.h"
#include "ResponseCurve.h"
#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomSlider : juce::Slider
{
    CustomSlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow) {}
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
    void EQAudioProcessorEditor::paint(juce::Graphics& g);

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
        peakFreqSliderAtt{ audioProcessor.apvts, "PeakFreq", peakFreqSlider },
        peakGainSliderAtt{ audioProcessor.apvts, "PeakGain", peakGainSlider },
        peakQualSliderAtt{ audioProcessor.apvts, "PeakQual", peakQualSlider },
        lowCutFreqSliderAtt{ audioProcessor.apvts, "LowCutFreq", lowCutFreqSlider },
        hiCutFreqSliderAtt{ audioProcessor.apvts, "HiCutFreq", hiCutFreqSlider },
        lowCutSlopeSliderAtt{ audioProcessor.apvts, "LowCutSlope", lowCutSlopeSlider },
        hiCutSlopeSliderAtt{ audioProcessor.apvts, "HiCutSlope", hiCutSlopeSlider };
    
    ResponseCurveComp responseCurveComp;
    std::vector<juce::Component*> getComponents();

    //MonoChain monochian;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessorEditor)
};
