#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"

struct Dot : juce::Component
{
	Dot();
	~Dot();

	void mouseDown(const juce::MouseEvent& ev) override;
	void mouseDrag(const juce::MouseEvent& ev) override;
	void paint(juce::Graphics& g) override;
	const static int dotSize = 20;

private:
	juce::ComponentBoundsConstrainer contrainer;
	juce::ComponentDragger drag;

};

struct DraggableDots :
	juce::Component,
	juce::AudioProcessorParameter::Listener,
	juce::Timer
{
	DraggableDots(EQAudioProcessor& p);
	~DraggableDots();

	//void parameterValueChanged(int parameterIndex, float newValue);
	
	void timerCallback() override;
	void resized() override;
	void paint(juce::Graphics& g) override;
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}
	void parameterValueChanged(int, float) override;

private:
	Dot hpf, peak1, peak2, peak3, lpf;

	//juce::CriticalSection scopedlock;
	juce::Atomic<bool> parametersChange{ true };
	EQAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableDots)
};