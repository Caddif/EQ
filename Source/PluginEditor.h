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
        this->setColour(ColourIds::thumbColourId, juce::Colours::silver);
        this->setColour(ColourIds::rotarySliderFillColourId, juce::Colours::silver.darker(0.5f));
    }
};

struct SlopeBox : juce::ComboBox
{
    SlopeBox(const juce::String& componentName = {}) : juce::ComboBox()
    {
        juce::String str;

        for (int i = 0; i < 4; ++i) {
            str << (12 + 12 * i);
            str << " db/Octave";
            this->addItem(str, i+1);
            str.clear();
        }
    }
};

struct FilterChoice : juce::ComboBox
{
    FilterChoice(const juce::String& componentName = {}) : juce::ComboBox()
    {
        this->addItem("Peak Filter", 1);
        this->addItem("Low Shelf Filter", 2);
        this->addItem("High Shelf Filter", 3);
        this->addItem("notch Filter", 4);
    }
};

struct Bypassbuttons : juce::TextButton
{
    Bypassbuttons(const juce::String& buttonText) : juce::TextButton(buttonText)
    {
        this->setClickingTogglesState(true);
        this->setAlwaysOnTop(true);
        this->setColour(juce::TextButton::buttonOnColourId, juce::Colours::darkred);
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

    Bypassbuttons
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
        ngThresholdSlider,
        ngRatioSlider,
        ngAttackSlider,
        ngReleaseSlider,

        compThresholdSlider,
        compRatioSlider,
        compAttackSlider,
        compReleaseSlider,

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
        totalGainSlider;

    juce::AudioProcessorValueTreeState::SliderAttachment
        ngThresholdSliderAtt{ audioProcessor.apvts, "ngThreshold", ngThresholdSlider },
        ngRatioSliderAtt{ audioProcessor.apvts, "ngRatio", ngRatioSlider },
        ngAttackSliderAtt{ audioProcessor.apvts, "ngAttack", ngAttackSlider },
        ngReleaseSliderAtt{ audioProcessor.apvts, "ngRelease", ngReleaseSlider },

        compThresholdSliderAtt{ audioProcessor.apvts, "compThreshold", compThresholdSlider },
        compRatioSliderAtt{ audioProcessor.apvts, "compRatio", compRatioSlider },
        compAttackSliderAtt{ audioProcessor.apvts, "compAttack", compAttackSlider },
        compReleaseSliderAtt{ audioProcessor.apvts, "compRelease", compReleaseSlider },

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
        totalGainAtt{ audioProcessor.apvts, "TotalGain", totalGainSlider };
    
    SlopeBox lpfSlope, hpfSlope;

    juce::AudioProcessorValueTreeState::ComboBoxAttachment
        lpfSlopeAtt{ audioProcessor.apvts, "LowPassSlope", lpfSlope },
        hpfSlopeAtt{ audioProcessor.apvts, "HiPassSlope", hpfSlope };

    FilterChoice Filter1choice, Filter2choice, Filter3choice;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment
        Filter1choiceAtt{ audioProcessor.apvts, "Filter1choice", Filter1choice },
        Filter2choiceAtt{ audioProcessor.apvts, "Filter2choice", Filter2choice },
        Filter3choiceAtt{ audioProcessor.apvts, "Filter3choice", Filter3choice };

    juce::Label bypasslabel{ "bypassLabel", "Bypass filters" };

    ResponseCurveComp responseCurveComp;
    DraggableDots dots;
    juce::ColourGradient gradient;

    std::vector<juce::Component*> getComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessorEditor)
};
