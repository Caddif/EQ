#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"

struct Dot : juce::Component
{
	Dot(int index);
	~Dot();

	void mouseDown(const juce::MouseEvent& ev) override;
	void mouseDrag(const juce::MouseEvent& ev) override;
	void paint(juce::Graphics& g) override;
	const static int dotSize = 30;
	std::function<void(juce::Point<double>)> moveCallback;

private:
	int id;
	juce::ComponentDragger drag;
	juce::ComponentBoundsConstrainer contrainer;
};

struct DraggableDots :
	juce::Component,
	juce::Slider::Listener,
	juce::Timer
{
	DraggableDots(EQAudioProcessor&);
	~DraggableDots();
	
	void resized() override;
	void registerSlider(juce::Slider* slider);

private:
	void timerCallback() override;
	void sliderValueChanged(juce::Slider* slider) override;

	const double logsum = (std::log(10)) * 3.0;
	juce::MathConstants<float> e_math;
	std::vector<juce::Slider*> sliderPtr;

	Dot hpf{ 0 }, filter1{ 1 }, filter2{ 2 }, filter3{ 3 }, lpf{ 4 };

	EQAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableDots)
};