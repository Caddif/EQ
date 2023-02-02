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

    for (auto* components : getSlidersAndButtons())
    {
        addAndMakeVisible(components);
    }

    for (auto* components : graphicalComponents())
    {
        addAndMakeVisible(components);
    }

    double ratio = 12.0/5.0;
    setSize(1200, 1200 / ratio);
    setResizable(true, false);
    getConstrainer()->setFixedAspectRatio(ratio);
    setResizeLimits(800, 800 / ratio, 1920, 1920 / ratio);

    dots.registerSlider(&hiPassFreqSlider);
    dots.registerSlider(&filter1FreqSlider);
    dots.registerSlider(&filter1GainSlider);
    dots.registerSlider(&filter2FreqSlider);
    dots.registerSlider(&filter2GainSlider);
    dots.registerSlider(&filter3FreqSlider);
    dots.registerSlider(&filter3GainSlider);
    dots.registerSlider(&lowPassFreqSlider);

    hpfBypass.setToggleState(true, true);
    lpfBypass.setToggleState(true, true);

    ngThresholdSlider.setTextValueSuffix(" dB");
    ngRatioSlider.setTextValueSuffix(":1");
    ngAttackSlider.setTextValueSuffix(" ms");
    ngReleaseSlider.setTextValueSuffix(" ms");

    compThresholdSlider.setTextValueSuffix(" dB");
    compRatioSlider.setTextValueSuffix(":1");
    compAttackSlider.setTextValueSuffix(" ms");
    compReleaseSlider.setTextValueSuffix(" ms");

    filter1FreqSlider.setTextValueSuffix(" Hz");
    filter2FreqSlider.setTextValueSuffix(" Hz");
    filter3FreqSlider.setTextValueSuffix(" Hz");

    filter1QualSlider.setTextValueSuffix(" Q");
    filter2QualSlider.setTextValueSuffix(" Q");
    filter3QualSlider.setTextValueSuffix(" Q");

    filter1GainSlider.setTextValueSuffix(" dB");
    filter2GainSlider.setTextValueSuffix(" dB");
    filter3GainSlider.setTextValueSuffix(" dB");

    lowPassFreqSlider.setTextValueSuffix(" Hz");
    hiPassFreqSlider.setTextValueSuffix(" Hz");

    totalGainSlider.setTextValueSuffix(" dB");

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
    noiseGateBypass.setBounds(noiseGateArea.removeFromBottom(noiseGateArea.getHeight() * 0.08).reduced(5));
    ngThresholdSlider.setBounds(noiseGateArea.removeFromTop(noiseGateArea.getHeight() * 0.25).reduced(5));
    ngRatioSlider.setBounds(noiseGateArea.removeFromTop(noiseGateArea.getHeight() * 0.33).reduced(5));
    ngAttackSlider.setBounds(noiseGateArea.removeFromTop(noiseGateArea.getHeight() * 0.5).reduced(5));
    ngReleaseSlider.setBounds(noiseGateArea.reduced(5));

    compressorBypass.setBounds(noiseGateAndComp.removeFromBottom(noiseGateAndComp.getHeight() * 0.08).reduced(5));
    compThresholdSlider.setBounds(noiseGateAndComp.removeFromTop(noiseGateAndComp.getHeight() * 0.25).reduced(5));
    compRatioSlider.setBounds(noiseGateAndComp.removeFromTop(noiseGateAndComp.getHeight() * 0.33).reduced(5));
    compAttackSlider.setBounds(noiseGateAndComp.removeFromTop(noiseGateAndComp.getHeight() * 0.5).reduced(5));
    compReleaseSlider.setBounds(noiseGateAndComp.reduced(5));

    //////////////////
    //Bypass section//
    //////////////////
    bypasslabel.setBounds(bypasses.reduced(3));

    bypasses.removeFromTop(bypasses.getHeight() * 0.4);
    bypasses.removeFromBottom(bypasses.getHeight() * 0.33);
    bypasses.removeFromLeft(15);

    bypasslabel.setFont(juce::Font(bypasses.getHeight()/1.85f));

    hpfBypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.2));
    filter1bypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.25));
    filter2bypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.33));
    filter3bypass.setBounds(bypasses.removeFromLeft(bypasses.getWidth() * 0.5));
    lpfBypass.setBounds(bypasses);
   
    filter1bypass.changeWidthToFitText();
    filter2bypass.changeWidthToFitText();
    filter3bypass.changeWidthToFitText();

    hpfBypass.setBounds(hpfBypass.getX(), hpfBypass.getY(), filter1bypass.getWidth(), filter1bypass.getHeight());
    lpfBypass.setBounds(lpfBypass.getX(), lpfBypass.getY(), filter1bypass.getWidth(), filter1bypass.getHeight());

    /////////////////////////
    //Responsegraph section//
    /////////////////////////
    responseGraphArea.reduce(1, 1);
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
    totalGainSlider.setBounds(gainArea.reduced(5));

    hiPassFreqSlider.setBounds(hiPassArea.removeFromLeft(static_cast<int>(hiPassArea.getWidth() * 0.50)).reduced(5));
    lowPassFreqSlider.setBounds(hiPassArea.reduced(5));

    filter1FreqSlider.setBounds(peakArea.removeFromTop(static_cast<int>(peakArea.getHeight() * 0.33)));
    filter1GainSlider.setBounds(peakArea.removeFromTop(static_cast<int>(peakArea.getHeight() * 0.5)));
    filter1QualSlider.setBounds(peakArea);

    filter2FreqSlider.setBounds(peak2Area.removeFromTop(static_cast<int>(peak2Area.getHeight() * 0.33)));
    filter2GainSlider.setBounds(peak2Area.removeFromTop(static_cast<int>(peak2Area.getHeight() * 0.5)));
    filter2QualSlider.setBounds(peak2Area);
    
    filter3FreqSlider.setBounds(peak3Area.removeFromTop(static_cast<int>(peak3Area.getHeight() * 0.33)));
    filter3GainSlider.setBounds(peak3Area.removeFromTop(static_cast<int>(peak3Area.getHeight() * 0.5)));
    filter3QualSlider.setBounds(peak3Area);
}

void EQAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    gradient.point1 = juce::Point<float>(0, 0);
    gradient.point2 = juce::Point<float>(bounds.getHeight(), bounds.getWidth());
    
    gradient.addColour(0, juce::Colours::steelblue.withBrightness(0.3f));
    gradient.addColour(1, juce::Colours::steelblue.withBrightness(0.6f));

    g.setGradientFill(gradient);
    g.fillRect(bounds);

    // outlines
    float linethicknes = 2.f;
    
    auto gateAndComp = bounds.removeFromLeft(bounds.getWidth() * 0.2);
    g.setColour(juce::Colours::steelblue.withBrightness(0.25f));
    g.drawRect(gateAndComp.removeFromLeft(gateAndComp.getWidth() * 0.5), linethicknes);
    g.drawRect(gateAndComp, linethicknes);

    auto slopes = bounds.removeFromBottom(bounds.getHeight() * 0.15);
    g.drawRect(slopes.removeFromRight(slopes.getWidth()*0.4), linethicknes);
    g.drawRect(slopes, linethicknes);
    g.drawRect(bounds.removeFromLeft(bounds.getWidth() * 0.6), linethicknes);
    
    auto gainAndFreqs = bounds.removeFromBottom(static_cast<int>(bounds.getWidth() * 0.33));
    g.drawRect(gainAndFreqs.removeFromRight(gainAndFreqs.getWidth() * 0.5), linethicknes);
    g.drawRect(gainAndFreqs, linethicknes);
    g.drawRect(bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.33)), linethicknes);
    g.drawRect(bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.50)), linethicknes);
    g.drawRect(bounds, linethicknes);

    paintlabels(g);
}

void EQAudioProcessorEditor::paintlabels(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.setFont(compRatioSlider.getWidth() / 7);

    g.drawText("Thold", ngThresholdSlider.getBounds(), juce::Justification::top);
    g.drawText("Thold", compThresholdSlider.getBounds(), juce::Justification::top);
    g.drawText("Ratio", ngRatioSlider.getBounds(), juce::Justification::top);
    g.drawText("Ratio", compRatioSlider.getBounds(), juce::Justification::top);
    g.drawText("Atck", ngAttackSlider.getBounds(), juce::Justification::top);
    g.drawText("Atck", compAttackSlider.getBounds(), juce::Justification::top);
    g.drawText("Rel", ngReleaseSlider.getBounds(), juce::Justification::top);
    g.drawText("Rel", compReleaseSlider.getBounds(), juce::Justification::top);

    g.drawText("Freq", filter1FreqSlider.getBounds().reduced(4), juce::Justification::top);
    g.drawText("Freq", filter2FreqSlider.getBounds().reduced(4), juce::Justification::top);
    g.drawText("Freq", filter3FreqSlider.getBounds().reduced(4), juce::Justification::top);
    g.drawText("Gain", filter1GainSlider.getBounds().reduced(4), juce::Justification::top);
    g.drawText("Gain", filter2GainSlider.getBounds().reduced(4), juce::Justification::top);
    g.drawText("Gain", filter3GainSlider.getBounds().reduced(4), juce::Justification::top);

    g.drawText("LPF slope", lpfSlope.getBounds().withTop(lpfSlope.getY() - (lpfSlope.getHeight() / 2) - 2), juce::Justification::centredTop);
    g.drawText("HPF slope", hpfSlope.getBounds().withTop(hpfSlope.getY() - (hpfSlope.getHeight() / 2) - 2), juce::Justification::centredTop);

    g.drawText("HPF", hiPassFreqSlider.getBounds().reduced(2), juce::Justification::top);
    g.drawText("LPF", lowPassFreqSlider.getBounds().reduced(2), juce::Justification::top);
    g.drawText("Gain", totalGainSlider.getBounds().reduced(2), juce::Justification::top);
}

std::vector<juce::Component*> EQAudioProcessorEditor::getSlidersAndButtons()
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

        &filter1FreqSlider,
        &filter1GainSlider,
        &filter1QualSlider,
        &filter2FreqSlider,
        &filter2GainSlider,
        &filter2QualSlider,
        &filter3FreqSlider,
        &filter3GainSlider,
        &filter3QualSlider,

        &hiPassFreqSlider,
        &lowPassFreqSlider,
        &totalGainSlider,

        &noiseGateBypass,
        &compressorBypass,
        &hpfBypass,
        &filter1bypass,
        &filter2bypass,
        &filter3bypass,
        &lpfBypass,

        &hpfSlope,
        &lpfSlope,

        &Filter1choice,
        &Filter2choice,
        &Filter3choice
    };
}

std::vector<juce::Component*> EQAudioProcessorEditor::graphicalComponents()
{
    return
    {
        &responseCurveComp,
        &dots,

        &bypasslabel,
        &thresholdGate
    };
}

