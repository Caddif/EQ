#include "DraggableDots.h"

Dot::Dot(int index)
{
	contrainer.setMinimumOnscreenAmounts(dotSize/2, dotSize/2, dotSize/2, dotSize/2);
	this->setAlwaysOnTop(true);
	this->setBounds(0, 0, dotSize, dotSize);
	this->id = index;
}

Dot::~Dot()
{}

void Dot::mouseDown(const juce::MouseEvent& ev)
{
	drag.startDraggingComponent(this, ev);
}

void Dot::mouseDrag(const juce::MouseEvent& ev)
{
	drag.dragComponent(this, ev, &contrainer);
	if (moveCallback)
		moveCallback(getPosition().toDouble());
}

void Dot::paint(juce::Graphics& g)
{
	float linesize = 2.f;

	g.setColour(juce::Colours::silver);
	g.drawEllipse(getLocalBounds().toFloat().reduced(2.f), linesize);
	g.drawFittedText(juce::String(id), getLocalBounds(), juce::Justification::centred, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
DraggableDots::DraggableDots(EQAudioProcessor& p) : audioProcessor(p)
{
	addAndMakeVisible(hpf);
	addAndMakeVisible(peak1);
	addAndMakeVisible(peak2);
	addAndMakeVisible(peak3);
	addAndMakeVisible(lpf);

	startTimerHz(144);

	hpf.moveCallback = [&](juce::Point<double> position)
	{
		const auto bounds = getLocalBounds().toDouble();
		sliderPtr[0]->setValue(20 * std::pow(e_math.euler, (3.f / bounds.getWidth()) * (hpf.getX()+hpf.dotSize/2) * (std::log(2) + std::log(5))));
	};

	peak1.moveCallback = [&](juce::Point<double> position)
	{
		const auto bounds = getLocalBounds().toDouble();
		sliderPtr[1]->setValue(20 * std::pow(e_math.euler, (3.f / bounds.getWidth()) * (peak1.getX() + peak1.dotSize / 2) * (std::log(2) + std::log(5))));
		sliderPtr[2]->setValue(juce::jmap(static_cast<double>(peak1.getY()), static_cast<double>(-peak1.dotSize/2), bounds.getHeight()-peak1.dotSize/2, 24.0, -24.0));
	};

	peak2.moveCallback = [&](juce::Point<double> position)
	{
		const auto bounds = getLocalBounds().toDouble();
		sliderPtr[3]->setValue(20 * std::pow(e_math.euler, (3.f / bounds.getWidth()) * (peak2.getX() + peak2.dotSize / 2) * (std::log(2) + std::log(5))));
		sliderPtr[4]->setValue(juce::jmap(static_cast<double>(peak2.getY()), static_cast<double>(-peak2.dotSize / 2), bounds.getHeight() - peak2.dotSize / 2, 24.0, -24.0));
	};

	peak3.moveCallback = [&](juce::Point<double> position)
	{
		const auto bounds = getLocalBounds().toDouble();
		sliderPtr[5]->setValue(20 * std::pow(e_math.euler, (3.f / bounds.getWidth()) * (peak3.getX() + peak3.dotSize / 2) * (std::log(2) + std::log(5))));
		sliderPtr[6]->setValue(juce::jmap(static_cast<double>(peak3.getY()), static_cast<double>(-peak3.dotSize / 2), bounds.getHeight() - peak3.dotSize / 2, 24.0, -24.0));
	};

	lpf.moveCallback = [&](juce::Point<double> position)
	{
		const auto bounds = getLocalBounds().toDouble();
		sliderPtr[7]->setValue(20 * std::pow(e_math.euler, (3.f / bounds.getWidth()) * (lpf.getX() + lpf.dotSize / 2) * (std::log(2) + std::log(5))));
	};
}

DraggableDots::~DraggableDots()
{
	for (auto i : sliderPtr)
	{
		i->removeListener(this);
	}
}

void DraggableDots::resized()
{
	auto chain = getChainSettings(audioProcessor.apvts);
	auto bounds = getLocalBounds();

	hpf.setTopLeftPosition((log10(chain.HiPassFreq / 20.f) / 3) * bounds.getWidth() - hpf.dotSize / 2, bounds.getHeight() / 2 - hpf.dotSize / 2);
	peak1.setTopLeftPosition(
		(log10(chain.filterFreq / 20.f) / 3) * bounds.getWidth() - hpf.dotSize / 2,
		juce::jmap(static_cast<double>(chain.gainInDb), 24.0, -24.0, static_cast<double>(-peak1.dotSize / 2), static_cast<double>(bounds.getHeight() - peak1.dotSize / 2)));
	peak2.setTopLeftPosition(
		(log10(chain.filterFreq2 / 20.f) / 3) * bounds.getWidth() - hpf.dotSize / 2,
		juce::jmap(static_cast<double>(chain.gainInDb2), 24.0, -24.0, static_cast<double>(-peak1.dotSize / 2), static_cast<double>(bounds.getHeight() - peak1.dotSize / 2)));
	peak3.setTopLeftPosition(
		(log10(chain.filterFreq3 / 20.f) / 3) * bounds.getWidth() - hpf.dotSize / 2,
		juce::jmap(static_cast<double>(chain.gainInDb3), 24.0, -24.0, static_cast<double>(-peak1.dotSize / 2), static_cast<double>(bounds.getHeight() - peak1.dotSize / 2)));
	lpf.setTopLeftPosition((log10(chain.lowPassFreq / 20.f) / 3) * bounds.getWidth() - lpf.dotSize / 2, bounds.getHeight() / 2 - lpf.dotSize / 2);
}

void DraggableDots::registerSlider(juce::Slider* slider)
{
	slider->addListener(this);
	sliderPtr.push_back(slider);
}

void DraggableDots::timerCallback()
{
	if (hpf.isMouseOverOrDragging(true))
		hpf.repaint();
	if (peak1.isMouseOverOrDragging(true))
		peak1.repaint();
	if (peak2.isMouseOverOrDragging(true))
		peak2.repaint();
	if (peak3.isMouseOverOrDragging(true))
		peak3.repaint();
	if (lpf.isMouseOverOrDragging(true))
		lpf.repaint();
}

void DraggableDots::sliderValueChanged(juce::Slider* slider)
{
	if (hpf.isMouseOverOrDragging(false) || 
		peak1.isMouseOverOrDragging(false) || 
		peak2.isMouseOverOrDragging(false) || 
		peak3.isMouseOverOrDragging(false) || 
		lpf.isMouseOverOrDragging(false))
		return;

	auto bounds = getLocalBounds();
	hpf.setTopLeftPosition((log10(sliderPtr[0]->getValue() / 20.f) / 3) * bounds.getWidth() - hpf.dotSize / 2, bounds.getHeight()/2-hpf.dotSize/2);
	peak1.setTopLeftPosition(
		(log10(sliderPtr[1]->getValue() / 20.f) / 3) * bounds.getWidth() - hpf.dotSize / 2, 
		juce::jmap(static_cast<double>(sliderPtr[2]->getValue()), 24.0, -24.0,static_cast<double>(-peak1.dotSize / 2), static_cast<double>(bounds.getHeight() - peak1.dotSize / 2)));
	peak2.setTopLeftPosition(
		(log10(sliderPtr[3]->getValue() / 20.f) / 3) * bounds.getWidth() - hpf.dotSize / 2,
		juce::jmap(static_cast<double>(sliderPtr[4]->getValue()), 24.0, -24.0, static_cast<double>(-peak1.dotSize / 2), static_cast<double>(bounds.getHeight() - peak1.dotSize / 2)));
	peak3.setTopLeftPosition(
		(log10(sliderPtr[5]->getValue() / 20.f) / 3) * bounds.getWidth() - hpf.dotSize / 2,
		juce::jmap(static_cast<double>(sliderPtr[6]->getValue()), 24.0, -24.0, static_cast<double>(-peak1.dotSize / 2), static_cast<double>(bounds.getHeight() - peak1.dotSize / 2)));
	lpf.setTopLeftPosition((log10(sliderPtr[7]->getValue() / 20.f) / 3) * bounds.getWidth() - lpf.dotSize / 2, bounds.getHeight() / 2 - lpf.dotSize / 2);
}