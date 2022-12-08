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
    auto hiPassArea = bounds.removeFromBottom(static_cast<int>(bounds.getWidth() * 0.33));
    auto lowPassArea = hiPassArea.removeFromRight(static_cast<int>(hiPassArea.getWidth() * 0.50));
    auto peakArea= bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.33));
    auto peak2Area = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.50));
    auto peak3Area = bounds;

    responseGraphArea.reduce(10, 10);
    responseCurveComp.setBounds(responseGraphArea);

    hiPassFreqSlider.setBounds(hiPassArea.removeFromLeft(static_cast<int>(hiPassArea.getWidth() * 0.50)));
    hiPassSlopeSlider.setBounds(hiPassArea);
    lowPassFreqSlider.setBounds(lowPassArea.removeFromLeft(static_cast<int>(lowPassArea.getWidth() * 0.50)));
    lowPassSlopeSlider.setBounds(lowPassArea);

    peakFreqSlider.setBounds(peakArea.removeFromTop(static_cast<int>(peakArea.getHeight() * 0.33)));
    peakGainSlider.setBounds(peakArea.removeFromTop(static_cast<int>(peakArea.getHeight() * 0.5)));
    peakQualSlider.setBounds(peakArea);

    peakFreqSlider2.setBounds(peak2Area.removeFromTop(static_cast<int>(peak2Area.getHeight() * 0.33)));
    peakGainSlider2.setBounds(peak2Area.removeFromTop(static_cast<int>(peak2Area.getHeight() * 0.5)));
    peakQualSlider2.setBounds(peak2Area);
    
    peakFreqSlider3.setBounds(peak3Area.removeFromTop(static_cast<int>(peak3Area.getHeight() * 0.33)));
    peakGainSlider3.setBounds(peak3Area.removeFromTop(static_cast<int>(peak3Area.getHeight() * 0.5)));
    peakQualSlider3.setBounds(peak3Area);

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
        &peakFreqSlider2,
        &peakGainSlider2,
        &peakQualSlider2,
        &peakFreqSlider3,
        &peakGainSlider3,
        &peakQualSlider3,
        &hiPassFreqSlider,
        &lowPassFreqSlider,
        &hiPassSlopeSlider,
        &lowPassSlopeSlider,
        &responseCurveComp
    };
}
