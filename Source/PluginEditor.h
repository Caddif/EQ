#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Params.h"
#include "ResponseGraph/ResponseGraph.h"
#include "DraggableDots/DraggableDots.h"

struct ParamSlider : juce::Slider
{
    ParamSlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow) 
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

    juce::ToggleButton
        hpfBypass{"HPF"},
        peak1Bypass{"Peak 1"},
        peak2Bypass{"Peak 2"},
        peak3Bypass{"Peak 3"},
        lpfBypass{"LPF"};

    juce::AudioProcessorValueTreeState::ButtonAttachment
        HPFbypassAtt{ audioProcessor.apvts, "HPFbypass", hpfBypass },
        Peak1bypassAtt{ audioProcessor.apvts, "Peak1bypass", peak1Bypass },
        Peak2bypassAtt{ audioProcessor.apvts, "Peak2bypass", peak2Bypass },
        Peak3bypassAtt{ audioProcessor.apvts, "Peak3bypass", peak3Bypass },
        LPFbypassAtt{ audioProcessor.apvts, "LPFbypass", lpfBypass };

    ParamSlider
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
    DraggableDots dots;
    juce::ColourGradient gradient;

    std::vector<juce::Component*> getComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessorEditor)
};
