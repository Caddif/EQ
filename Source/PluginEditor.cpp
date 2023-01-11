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

    setSize (1200, 500);
    setResizable(true, false);
    dots.registerSlider(&hiPassFreqSlider);
    dots.registerSlider(&peakFreqSlider);
    dots.registerSlider(&peakGainSlider);
    dots.registerSlider(&peakFreqSlider2);
    dots.registerSlider(&peakGainSlider2);
    dots.registerSlider(&peakFreqSlider3);
    dots.registerSlider(&peakGainSlider3);
    dots.registerSlider(&lowPassFreqSlider);
    dots.registerSlider(&peakQualSlider);

    hpfBypass.setToggleState(true, true);
    lpfBypass.setToggleState(true, true);

    bypasslabel.setJustificationType(juce::Justification::topLeft);
}

EQAudioProcessorEditor::~EQAudioProcessorEditor(){}
//==============================================================================


void EQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto noiseGateAndComp = bounds.removeFromLeft(bounds.getWidth() * 0.2);
    auto bypasses = bounds.removeFromBottom(bounds.getHeight() * 0.15);
    auto responseGraphArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.6));
    auto hiPassArea = bounds.removeFromBottom(static_cast<int>(bounds.getWidth() * 0.33));
    auto gainArea = hiPassArea.removeFromRight(static_cast<int>(hiPassArea.getWidth() * 0.50));
    auto filterchoiceArea = bounds.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.15));
    auto peakArea= bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.33));
    auto peak2Area = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.50));
    auto peak3Area = bounds;

    auto comboBoxes = bypasses.removeFromRight(bypasses.getWidth() * 0.4);

    /////////////////////
    //NG & Comp section//
    /////////////////////
    auto noiseGateArea = noiseGateAndComp.removeFromLeft(noiseGateAndComp.getWidth() * 0.5);
    ngThresholdSlider.setBounds(noiseGateArea.removeFromTop(noiseGateArea.getHeight() * 0.25));
    ngRatioSlider.setBounds(noiseGateArea.removeFromTop(noiseGateArea.getHeight() * 0.33));
    ngAttackSlider.setBounds(noiseGateArea.removeFromTop(noiseGateArea.getHeight() * 0.5));
    ngReleaseSlider.setBounds(noiseGateArea);

    compThresholdSlider.setBounds(noiseGateAndComp.removeFromTop(noiseGateAndComp.getHeight() * 0.25));
    compRatioSlider.setBounds(noiseGateAndComp.removeFromTop(noiseGateAndComp.getHeight() * 0.33));
    compAttackSlider.setBounds(noiseGateAndComp.removeFromTop(noiseGateAndComp.getHeight() * 0.5));
    compReleaseSlider.setBounds(noiseGateAndComp);

    //////////////////
    //Bypass section//
    //////////////////
    bypasslabel.setBounds(bypasses);
    bypasslabel.setFont(juce::Font(18.f));

    bypasses.removeFromTop(bypasses.getHeight() * 0.4);
    bypasses.removeFromBottom(bypasses.getHeight() * 0.33);
    bypasses.removeFromLeft(15);

    hpfBypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.2));
    peak1Bypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.25));
    peak2Bypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.33));
    peak3Bypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.5));
    lpfBypass.setBounds(bypasses);

    hpfBypass.changeWidthToFitText();
    peak1Bypass.changeWidthToFitText();
    peak2Bypass.changeWidthToFitText();
    peak3Bypass.changeWidthToFitText();
    lpfBypass.changeWidthToFitText();


    /////////////////////////
    //Responsegraph section//
    /////////////////////////
    responseGraphArea.reduce(10, 10);
    responseCurveComp.setBounds(responseGraphArea);
    dots.setBounds(responseGraphArea);

    ////////////////////
    //ComboBox section//
    ////////////////////
    auto hpf = comboBoxes.removeFromLeft(comboBoxes.getWidth() * 0.5);
    auto lpf = comboBoxes;
    hpfSlope.setBounds(hpf.reduced(hpf.getWidth() * 0.2, hpf.getHeight() * 0.3));
    lpfSlope.setBounds(lpf.reduced(lpf.getWidth() * 0.2, lpf.getHeight() * 0.3));

    /////////////////////////
    //Filter Choice section//
    /////////////////////////
    auto f1 = filterchoiceArea.removeFromLeft(filterchoiceArea.getWidth() * 0.33);
    auto f2 = filterchoiceArea.removeFromLeft(filterchoiceArea.getWidth() * 0.5);
    auto f3 = filterchoiceArea;
    Filter1choice.setBounds(f1.reduced(f1.getWidth() * 0.1, f1.getHeight() * 0.2));
    Filter2choice.setBounds(f2.reduced(f2.getWidth() * 0.1, f2.getHeight() * 0.2));
    Filter3choice.setBounds(f3.reduced(f3.getWidth() * 0.1, f3.getHeight() * 0.2));

    //////////////////
    //Slider section//
    //////////////////
    totalGainSlider.setBounds(gainArea);

    hiPassFreqSlider.setBounds(hiPassArea.removeFromLeft(static_cast<int>(hiPassArea.getWidth() * 0.50)));
    lowPassFreqSlider.setBounds(hiPassArea);

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
        &ngThresholdSlider,
        &ngRatioSlider,
        &ngAttackSlider,
        &ngReleaseSlider,

        &compThresholdSlider,
        &compRatioSlider,
        &compAttackSlider,
        &compReleaseSlider,


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
        &totalGainSlider,
        &responseCurveComp,

        &hpfBypass,
        &peak1Bypass,
        &peak2Bypass,
        &peak3Bypass,
        &lpfBypass,

        &dots,

        &bypasslabel,

        &hpfSlope,
        &lpfSlope,

        &Filter1choice,
        &Filter2choice,
        &Filter3choice
    };
}

