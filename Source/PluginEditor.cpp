#include "PluginProcessor.h"
#include "PluginEditor.h"

// Beneth EQAudioProcessorEditor 
//==============================================================================
EQAudioProcessorEditor::EQAudioProcessorEditor (EQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    responseCurveComp(audioProcessor),
    dots(audioProcessor)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    for (auto* components : getComponents())
    {
        addAndMakeVisible(components);
    }

    setSize (1000, 500);
    setResizable(true, false);
}

EQAudioProcessorEditor::~EQAudioProcessorEditor(){}
//==============================================================================


void EQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto bypasses = bounds.removeFromBottom(bounds.getHeight() * 0.2);
    auto responseGraphArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.6));
    auto hiPassArea = bounds.removeFromBottom(static_cast<int>(bounds.getWidth() * 0.33));
    auto lowPassArea = hiPassArea.removeFromRight(static_cast<int>(hiPassArea.getWidth() * 0.50));
    auto peakArea= bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.33));
    auto peak2Area = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.50));
    auto peak3Area = bounds;

    auto gainArea = bypasses.removeFromRight(bypasses.getWidth() * 0.4);

    bypasses.removeFromTop(bypasses.getHeight() * 0.33);
    bypasses.removeFromBottom(bypasses.getHeight() * 0.5);
    bypasses.removeFromLeft(20);

    hpfBypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.2));
    peak1Bypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.25));
    peak2Bypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.33));
    peak3Bypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.5));
    lpfBypass.setBounds(bypasses);

    responseGraphArea.reduce(10, 10);
    responseCurveComp.setBounds(responseGraphArea);
    dots.setBounds(responseGraphArea);

    hiPassFreqSlider.setBounds(hiPassArea.removeFromLeft(static_cast<int>(hiPassArea.getWidth() * 0.50)));
    hiPassSlopeSlider.setBounds(hiPassArea);

    totalGainSlider.setBounds(gainArea);

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
    auto bounds = getLocalBounds();

    gradient.point1 = juce::Point<float>(0, 0);
    gradient.point2 = juce::Point<float>(bounds.getHeight(), bounds.getWidth());
    
    gradient.addColour(0, juce::Colours::steelblue.withBrightness(0.3f));
    gradient.addColour(1, juce::Colours::steelblue.withBrightness(0.6f));

    g.setGradientFill(gradient);
    g.fillRect(bounds);
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
        &totalGainSlider,
        &responseCurveComp,
        &hpfBypass,
        &peak1Bypass,
        &peak2Bypass,
        &peak3Bypass,
        &lpfBypass,
        &dots
    };
}

