/*
  ==============================================================================

    ToneCircle.cpp
    Created: 5 Aug 2020 11:14:32pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "ToneCircle.h"

//==============================================================================
ToneCircle::ToneCircle(juce::String componentName)
	: juce::Component(componentName)
{
	// Set Default Colours
	setColour(circleOutlineColourId,  juce::Colours::whitesmoke);
	setColour(intervalStartColourId,  juce::Colours::red);
	setColour(intervalMiddleColourId, juce::Colours::red);
	setColour(intervalEndColourId,    juce::Colours::red);
}

ToneCircle::~ToneCircle()
{
}

void ToneCircle::paint(juce::Graphics& g)
{
	juce::Colour backgroundColour = juce::Colours::darkslategrey;
	g.fillAll(backgroundColour);
	
	g.setColour(juce::Colours::black);
	g.drawRect(getLocalBounds(), 1);

	juce::Colour circleColour = findColour(ColourIds::circleOutlineColourId);
	g.setColour(circleColour);
	g.drawEllipse(circleBounds, 1);

	juce::Colour intervalStart = findColour(ColourIds::intervalStartColourId);
	juce::Colour intervalMiddle = findColour(ColourIds::intervalMiddleColourId);
	juce::Colour intervalEnd = findColour(ColourIds::intervalEndColourId);

	intervalTextGradient = juce::ColourGradient(
		intervalStart, degreePoints[0],
		intervalMiddle, degreePoints[degreePoints.size() / 2],
		true
	);

	//juce::Font font = juce::Font(juce::Font::getDefaultMonospacedFontName(), getHeight() * 0.02, juce::Font::FontStyleFlags::plain);
	//g.setColour(circleColour);
	//auto periodText = "Period: " + tuning->getPeriodString();
	//auto periodWidth = font.getStringWidth(periodText) * 1.5f;
	//g.drawFittedText(periodText, juce::Rectangle<int>(5, 5, periodWidth, font.getHeight()), juce::Justification::centredLeft, 1);

	for (int i = 0; i < degreeAngles.size(); i++)
	{
		float ang = degreeAngles[i];
		juce::Point<float> circleCoord = degreePoints[i];

		g.setColour(circleColour);
		g.fillEllipse(circleCoord.getX() - noteMarkerRadius / 2, +circleCoord.getY() - noteMarkerRadius / 2, noteMarkerRadius, noteMarkerRadius);
	} 

	//for (auto noteNum : notesOn)
	//{
	//	int degree = tuning.getScaleDegree(noteNum);
	//	Point<float> circleCoord = degreePoints[degree];
	//	Colour intervalColour = intervalTextGradient.getColourAtPosition(degreeAngles[degree] / float_Pi * 2);

	//	g.setColour(intervalColour.brighter().withAlpha(0.9f));
	//	g.fillEllipse(circleCoord.getX() - noteMarkerRadius / 2, +circleCoord.getY() - noteMarkerRadius / 2, noteMarkerRadius, noteMarkerRadius);
	//}
}

void ToneCircle::resized()
{
	center = { getWidth() / 2.0f, getHeight() / 2.0f };
	circleRadius = juce::jmin(getWidth(), getHeight()) * (1 - margin) / 2.0f;
	circleBounds = { center.getX() - circleRadius, center.getY() - circleRadius, circleRadius * 2, circleRadius * 2};

	degreePoints.clear();
	for (auto ang : degreeAngles)
	{
		degreePoints.add(center.getPointOnCircumference(circleRadius, ang));
	}
}

void ToneCircle::setTuning(const MappedTuningTable* tuningIn)
{
	tuning = tuningIn;

	double period = tuning->getVirtualPeriod();
	if (period == 0)
		period = 1200.0;

	int size = tuning->getVirtualSize();
	if (size == 0)
		size = tuning->getTuningSize();

	degreeAngles.clear();
	for (int i = 0; i < size; i++)
	{
		degreeAngles.add(scaleDegreeToAngle(i, period));
	}

	resized();
}

float ToneCircle::scaleDegreeToAngle(int scaleDegreeIn, double period) const
{
	return 2.0f * juce::float_Pi * tuning->centsFromRoot(scaleDegreeIn) / 1200;
}