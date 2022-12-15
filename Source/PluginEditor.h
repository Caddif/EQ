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
    CustomSlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow) 
    {
        juce::Slider::setColour(ColourIds::thumbColourId, juce::Colours::silver);
        juce::Slider::setColour(ColourIds::rotarySliderFillColourId, juce::Colours::silver.darker(0.5f));
    }
};

class EQAudioProcessorEditor  : 
    public juce::AudioProcessorEditor
{
public:
    EQAudioProcessorEditor (EQAudioProcessor&);
    ~EQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EQAudioProcessor& audioProcessor;

    CustomSlider
        peakFreqSlider,
        peakGainSlider, 
        peakQualSlider, 
        peakFreqSlider2,
        peakGainSlider2,
        peakQualSlider2,
        peakFreqSlider3,
        peakGainSlider3,
        peakQualSlider3,
        hiPassFreqSlider, 
        lowPassFreqSlider, 
        hiPassSlopeSlider, 
        lowPassSlopeSlider,
        totalGainSlider;
    
    juce::AudioProcessorValueTreeState::SliderAttachment
        peakFreqSliderAtt{ audioProcessor.apvts, "PeakFreq", peakFreqSlider },
        peakGainSliderAtt{ audioProcessor.apvts, "PeakGain", peakGainSlider },
        peakQualSliderAtt{ audioProcessor.apvts, "PeakQual", peakQualSlider },
        peakFreqSliderAtt2{ audioProcessor.apvts, "PeakFreq2", peakFreqSlider2 },
        peakGainSliderAtt2{ audioProcessor.apvts, "PeakGain2", peakGainSlider2 },
        peakQualSliderAtt2{ audioProcessor.apvts, "PeakQual2", peakQualSlider2 },
        peakFreqSliderAtt3{ audioProcessor.apvts, "PeakFreq3", peakFreqSlider3 },
        peakGainSliderAtt3{ audioProcessor.apvts, "PeakGain3", peakGainSlider3 },
        peakQualSliderAtt3{ audioProcessor.apvts, "PeakQual3", peakQualSlider3 },
        hiPassFreqSliderAtt{ audioProcessor.apvts, "HiPassFreq", hiPassFreqSlider },
        lowPassFreqSliderAtt{ audioProcessor.apvts, "LowPassFreq", lowPassFreqSlider },
        hiPassSlopeSliderAtt{ audioProcessor.apvts, "HiPassSlope", hiPassSlopeSlider },
        lowPassSlopeSliderAtt{ audioProcessor.apvts, "LowPassSlope", lowPassSlopeSlider },
        totalGainAtt{ audioProcessor.apvts, "TotalGain", totalGainSlider };
    
    ResponseCurveComp responseCurveComp;
    juce::ColourGradient gradient;

    std::vector<juce::Component*> getComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessorEditor)
};
