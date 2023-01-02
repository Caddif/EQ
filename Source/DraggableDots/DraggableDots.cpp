#include "DraggableDots.h"

Dot::Dot()
{
	contrainer.setMinimumOnscreenAmounts(dotSize/2, dotSize/2, dotSize/2, dotSize/2);
	this->setAlwaysOnTop(true);
}

Dot::~Dot()
{

}

void Dot::mouseDown(const juce::MouseEvent& ev)
{
	drag.startDraggingComponent(this, ev);
}

void Dot::mouseDrag(const juce::MouseEvent& ev)
{
	drag.dragComponent(this, ev, &contrainer);
}

void Dot::paint(juce::Graphics& g)
{
	auto bound = getLocalBounds().toFloat();
	int linesize = 3;
	bound.reduce(linesize, linesize);

	g.setColour(juce::Colours::silver);
	g.drawEllipse(bound, static_cast<float>(linesize));
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

DraggableDots::DraggableDots(EQAudioProcessor& p) : audioProcessor(p)
{
	const auto& params = audioProcessor.getParameters();
	for (auto i : params)
	{
		i->addListener(this);
	}

	addAndMakeVisible(hpf);
	addAndMakeVisible(peak1);
	addAndMakeVisible(peak2);
	addAndMakeVisible(peak3);
	addAndMakeVisible(lpf);

	startTimerHz(60);
}

DraggableDots::~DraggableDots()
{
	const auto& params = audioProcessor.getParameters();
	for (auto i : params)
	{
		i->removeListener(this);
	}
}

void DraggableDots::timerCallback()
{
	if (parametersChange.compareAndSetBool(false, true))
	{
		auto chainsettings = getChainSettings(audioProcessor.apvts);
		auto bounds = getLocalBounds();

		float remapedHpfFreq = juce::jmap(chainsettings.HiPassFreq, 20.f, 20000.f, -hpf.dotSize / 2.f, static_cast<float>(bounds.getWidth()) - hpf.dotSize / 2);
		
		juce::NormalisableRange<float> targetRange{ -10.f, static_cast<float>(bounds.getWidth())-10.f };
		targetRange.setSkewForCentre(static_cast<float>(bounds.getWidth())*0.7f);

		const juce::NormalisableRange<float> sourceRange{ 20.f, 20000.f };
		const auto sourceValue = chainsettings.HiPassFreq;

		const auto normalisedValue = sourceRange.convertTo0to1(sourceValue);

		const auto targetValue = targetRange.convertFrom0to1(normalisedValue);

		hpf.setBounds(targetValue, bounds.getHeight()/2.f-10.f, hpf.dotSize, hpf.dotSize);
		
	}
}

void DraggableDots::resized()
{
	auto bounds = getLocalBounds();

	auto chainsettings = getChainSettings(audioProcessor.apvts);

	hpf.setBounds(0, bounds.getHeight()/2-hpf.dotSize/2, hpf.dotSize, hpf.dotSize);
	lpf.setBounds(0, 0, lpf.dotSize,lpf.dotSize);

	//jmap(chainsettings.HiPassFreq, 20.f, 20000.f, 0.f, static_cast<float>(bounds.getHeight()));
}

void DraggableDots::paint(juce::Graphics& g)
{
	
}

void DraggableDots::parameterValueChanged(int, float) 
{
	parametersChange.set(true);
}