/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"

// Beneth EQAudioProcessorEditor 
//==============================================================================
EQAudioProcessorEditor::EQAudioProcessorEditor (EQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    responseCurveComp(audioProcessor)
    /*peakFreqSliderAtt(audioProcessor.apvts, "PeakFreq", peakFreqSlider),
    peakGainSliderAtt( audioProcessor.apvts, "PeakGain", peakGainSlider ),
    peakQualSliderAtt( audioProcessor.apvts, "PeakQual", peakQualSlider ),
    lowCutFreqSliderAtt( audioProcessor.apvts, "LowCutFreq", lowCutFreqSlider ),
    hiCutFreqSliderAtt( audioProcessor.apvts, "HiCutFreq", hiCutFreqSlider ),
    lowCutSlopeSliderAtt( audioProcessor.apvts, "LowCutSlope", lowCutSlopeSlider ),
    hiCutSlopeSliderAtt( audioProcessor.apvts, "HiCutSlope", hiCutSlopeSlider )*/
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    for (auto* components : getComponents())
    {
        addAndMakeVisible(components);
    }

    setSize (1000, 400);
}

EQAudioProcessorEditor::~EQAudioProcessorEditor(){}
//==============================================================================

void EQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto responseGraphArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.5));
    auto lowCutArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.33));
    auto hiCutArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.50));

    responseCurveComp.setBounds(responseGraphArea);

    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(static_cast<int>(bounds.getHeight() * 0.50)));
    lowCutSlopeSlider.setBounds(lowCutArea);
    hiCutFreqSlider.setBounds(hiCutArea.removeFromTop(static_cast<int>(bounds.getHeight() * 0.50)));
    hiCutSlopeSlider.setBounds(hiCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.33)));
    peakGainSlider.setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.50)));
    peakQualSlider.setBounds(bounds);

}

void EQAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::steelblue.withBrightness(0.4f));
}

std::vector<juce::Component*> EQAudioProcessorEditor::getComponents()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualSlider,
        &lowCutFreqSlider,
        &hiCutFreqSlider,
        &lowCutSlopeSlider,
        &hiCutSlopeSlider,
        &responseCurveComp
    };
}
