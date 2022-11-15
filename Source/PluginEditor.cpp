/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EQAudioProcessorEditor::EQAudioProcessorEditor (EQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    for (auto* components : getComponents())
    {
        addAndMakeVisible(components);
    }

    setSize (1000, 600);
}

EQAudioProcessorEditor::~EQAudioProcessorEditor()
{
}

//==============================================================================
void EQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::left, 1);
}

void EQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto responseGraphArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.5));
    auto lowCutArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.33));
    auto hiCutArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.50));

    lowCutFreqSlider.setBounds(lowCutArea);
    hiCutFreqSlider.setBounds(hiCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.33)));
    peakGainSlider.setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.50)));
    peakQualSlider.setBounds(bounds);

}

std::vector<juce::Component*> EQAudioProcessorEditor::getComponents()
{
    //return std::vector<juce::Component*>();
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualSlider,
        &lowCutFreqSlider,
        &hiCutFreqSlider
    };
}
