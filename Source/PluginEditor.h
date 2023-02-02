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
        this->setColour(textBoxOutlineColourId, juce::Colours::silver.withAlpha(0.f));
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
        this->addItem("Notch Filter", 4);
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
    void paintlabels(juce::Graphics& g);
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EQAudioProcessor& audioProcessor;

    Bypassbuttons
        noiseGateBypass{"Noise Gate"},
        compressorBypass{"Compressor"},
        hpfBypass{"HPF"},
        filter1bypass{"Filter 1"},
        filter2bypass{"Filter 2"},
        filter3bypass{"Filter 3"},
        lpfBypass{"LPF"};

    juce::AudioProcessorValueTreeState::ButtonAttachment
        noiseGateBypassAtt{ audioProcessor.apvts, "ngBypass", noiseGateBypass },
        compressorBypassAtt{ audioProcessor.apvts, "compBypass", compressorBypass },
        HPFbypassAtt{ audioProcessor.apvts, "HPFbypass", hpfBypass },
        Peak1bypassAtt{ audioProcessor.apvts, "Filter1bypass", filter1bypass },
        Peak2bypassAtt{ audioProcessor.apvts, "Filter2bypass", filter2bypass },
        Peak3bypassAtt{ audioProcessor.apvts, "Filter3bypass", filter3bypass },
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

        filter1FreqSlider,
        filter1GainSlider,
        filter1QualSlider,
        filter2FreqSlider,
        filter2GainSlider,
        filter2QualSlider,
        filter3FreqSlider,
        filter3GainSlider,
        filter3QualSlider,

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

        filter1FreqSliderAtt{ audioProcessor.apvts, "Filter1Freq", filter1FreqSlider },
        filter1GainSliderAtt{ audioProcessor.apvts, "Filter1Gain", filter1GainSlider },
        filter1QualSliderAtt{ audioProcessor.apvts, "Filter1Qual", filter1QualSlider },
        filter2FreqSliderAtt{ audioProcessor.apvts, "Filter2Freq", filter2FreqSlider },
        filter2GainSliderAtt{ audioProcessor.apvts, "Filter2Gain", filter2GainSlider },
        filter2QualSliderAtt{ audioProcessor.apvts, "Filter2Qual", filter2QualSlider },
        filter3FreqSliderAtt{ audioProcessor.apvts, "Filter3Freq", filter3FreqSlider },
        filter3GainSliderAtt{ audioProcessor.apvts, "Filter3Gain", filter3GainSlider },
        filter3QualSliderAtt{ audioProcessor.apvts, "Filter3Qual", filter3QualSlider },

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

    juce::Label 
        bypasslabel{ "bypassLabel", "Bypass filters" },
        thresholdGate{ "thresholdGate", "threshold" };

    ResponseCurveComp responseCurveComp;
    DraggableDots dots;
    juce::ColourGradient gradient;

    std::vector<juce::Component*> getSlidersAndButtons();
    std::vector<juce::Component*> graphicalComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessorEditor)
};
