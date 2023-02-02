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
	addAndMakeVisible(filter1);
	addAndMakeVisible(filter2);
	addAndMakeVisible(filter3);
	addAndMakeVisible(lpf);

	startTimerHz(60);

	hpf.moveCallback = [&](juce::Point<double> position)
	{
		const auto bounds = getLocalBounds().toDouble();
		sliderPtr[0]->setValue(20 * std::pow(e_math.euler, ((hpf.getX() + hpf.dotSize / 2) / (bounds.getWidth())) * logsum));
	};

	filter1.moveCallback = [&](juce::Point<double> position)
	{
		const auto bounds = getLocalBounds().toDouble();
		sliderPtr[1]->setValue(20 * std::pow(e_math.euler, ((filter1.getX() + hpf.dotSize / 2) / (bounds.getWidth())) * logsum));
		sliderPtr[2]->setValue(juce::jmap(static_cast<double>(filter1.getY()), static_cast<double>(-filter1.dotSize/2), bounds.getHeight()- filter1.dotSize/2, 24.0, -24.0));
	};

	filter2.moveCallback = [&](juce::Point<double> position)
	{
		const auto bounds = getLocalBounds().toDouble();
		sliderPtr[3]->setValue(20 * std::pow(e_math.euler, ((filter2.getX() + hpf.dotSize / 2) / (bounds.getWidth())) * logsum));
		sliderPtr[4]->setValue(juce::jmap(static_cast<double>(filter2.getY()), static_cast<double>(-filter2.dotSize / 2), bounds.getHeight() - filter2.dotSize / 2, 24.0, -24.0));
	};

	filter3.moveCallback = [&](juce::Point<double> position)
	{
		const auto bounds = getLocalBounds().toDouble();
		sliderPtr[5]->setValue(20 * std::pow(e_math.euler, ((filter3.getX() + hpf.dotSize / 2) / (bounds.getWidth())) * logsum));
		sliderPtr[6]->setValue(juce::jmap(static_cast<double>(filter3.getY()), static_cast<double>(-filter3.dotSize / 2), bounds.getHeight() - filter3.dotSize / 2, 24.0, -24.0));
	};

	lpf.moveCallback = [&](juce::Point<double> position)
	{
		const auto bounds = getLocalBounds().toDouble();
		sliderPtr[7]->setValue(20 * std::pow(e_math.euler, ((lpf.getX() + hpf.dotSize / 2) / (bounds.getWidth())) * logsum));
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
	auto chain = getEqChainSettings(audioProcessor.apvts);
	auto bounds = getLocalBounds();

	hpf.setTopLeftPosition((log10(chain.HiPassFreq / 20.f) / 3) * bounds.getWidth() - hpf.dotSize / 2, bounds.getHeight() / 2 - hpf.dotSize / 2);
	filter1.setTopLeftPosition(
		(log10(chain.filterFreq / 20.f) / 3) * bounds.getWidth() - filter1.dotSize / 2,
		juce::jmap(static_cast<double>(chain.gainInDb), 24.0, -24.0, static_cast<double>(-filter1.dotSize / 2), static_cast<double>(bounds.getHeight() - filter1.dotSize / 2)));
	filter2.setTopLeftPosition(
		(log10(chain.filterFreq2 / 20.f) / 3) * bounds.getWidth() - filter2.dotSize / 2,
		juce::jmap(static_cast<double>(chain.gainInDb2), 24.0, -24.0, static_cast<double>(-filter2.dotSize / 2), static_cast<double>(bounds.getHeight() - filter2.dotSize / 2)));
	filter3.setTopLeftPosition(
		(log10(chain.filterFreq3 / 20.f) / 3) * bounds.getWidth() - filter3.dotSize / 2,
		juce::jmap(static_cast<double>(chain.gainInDb3), 24.0, -24.0, static_cast<double>(-filter3.dotSize / 2), static_cast<double>(bounds.getHeight() - filter3.dotSize / 2)));
	lpf.setTopLeftPosition((log10(chain.lowPassFreq / 20.f) / 3) * bounds.getWidth() - lpf.dotSize / 2, bounds.getHeight() / 2 - lpf.dotSize / 2);
}

void DraggableDots::registerSlider(juce::Slider* slider)
{
	slider->addListener(this);
	sliderPtr.push_back(slider);
}

void DraggableDots::timerCallback()
{
	if (hpf.isMouseButtonDown())
		hpf.repaint();
	if (filter1.isMouseButtonDown())
		filter1.repaint();
	if (filter2.isMouseButtonDown())
		filter2.repaint();
	if (filter3.isMouseButtonDown())
		filter3.repaint();
	if (lpf.isMouseButtonDown())
		lpf.repaint();
}

void DraggableDots::sliderValueChanged(juce::Slider* slider)
{
	if (hpf.isMouseButtonDown() || 
		filter1.isMouseButtonDown() ||
		filter2.isMouseButtonDown() ||
		filter3.isMouseButtonDown() ||
		lpf.isMouseButtonDown())
		return;

	auto bounds = getLocalBounds();
	hpf.setTopLeftPosition((log10(sliderPtr[0]->getValue() / 20.f) / 3) * bounds.getWidth() - hpf.dotSize / 2, bounds.getHeight()/2-hpf.dotSize/2);
	filter1.setTopLeftPosition(
		(log10(sliderPtr[1]->getValue() / 20.f) / 3) * bounds.getWidth() - filter1.dotSize / 2,
		juce::jmap(static_cast<double>(sliderPtr[2]->getValue()), 24.0, -24.0,static_cast<double>(-filter1.dotSize / 2), static_cast<double>(bounds.getHeight() - filter1.dotSize / 2)));
	filter2.setTopLeftPosition(
		(log10(sliderPtr[3]->getValue() / 20.f) / 3) * bounds.getWidth() - filter2.dotSize / 2,
		juce::jmap(static_cast<double>(sliderPtr[4]->getValue()), 24.0, -24.0, static_cast<double>(-filter2.dotSize / 2), static_cast<double>(bounds.getHeight() - filter2.dotSize / 2)));
	filter3.setTopLeftPosition(
		(log10(sliderPtr[5]->getValue() / 20.f) / 3) * bounds.getWidth() - filter3.dotSize / 2,
		juce::jmap(static_cast<double>(sliderPtr[6]->getValue()), 24.0, -24.0, static_cast<double>(-filter3.dotSize / 2), static_cast<double>(bounds.getHeight() - filter3.dotSize / 2)));
	lpf.setTopLeftPosition((log10(sliderPtr[7]->getValue() / 20.f) / 3) * bounds.getWidth() - lpf.dotSize / 2, bounds.getHeight() / 2 - lpf.dotSize / 2);
}