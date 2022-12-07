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
    auto responseGraphArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.6));
    auto lowCutArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.33));
    auto hiCutArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.50));

    responseCurveComp.setBounds(responseGraphArea);

    hiPassFreqSlider.setBounds(lowCutArea.removeFromTop(static_cast<int>(bounds.getHeight() * 0.50)));
    hiPassSlopeSlider.setBounds(lowCutArea);
    lowPassFreqSlider.setBounds(hiCutArea.removeFromTop(static_cast<int>(bounds.getHeight() * 0.50)));
    lowPassSlopeSlider.setBounds(hiCutArea);

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
        &hiPassFreqSlider,
        &lowPassFreqSlider,
        &hiPassSlopeSlider,
        &lowPassSlopeSlider,
        &responseCurveComp
    };
}
