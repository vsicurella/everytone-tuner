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

	auto font = juce::Font(juce::Font::getDefaultMonospacedFontName(), 12, juce::Font::FontStyleFlags::plain);
	intervalLabel = std::make_unique<juce::Label>("IntervalLabel");
	intervalLabel->setJustificationType(juce::Justification::centred);
	intervalLabel->setFont(font);
	addChildComponent(*intervalLabel);
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

	//double fontHeight = getHeight() * 0.02f;
	//juce::Font font = juce::Font(juce::Font::getDefaultMonospacedFontName(), fontHeight, juce::Font::FontStyleFlags::plain);
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

	int fontHeight = round(getHeight() * 0.05);
	intervalLabel->setBounds(getLocalBounds().withSizeKeepingCentre(getWidth(), fontHeight));

	degreePoints.clear();
	for (int i = 0; i < degreeAngles.size(); i++)
	{
		auto ang = degreeAngles[i];
		auto point = center.getPointOnCircumference(circleRadius, ang);
		degreePoints.add(point);
		//DBG(juce::String(i) + ": " + juce::String(ang) + ", " + point.toString());
	}
}

void ToneCircle::updateScale(const CentsDefinition& scaleToCopy)
{
	centsScale = scaleToCopy;

	double period = tuning->getVirtualPeriod();
	if (period == 0)
		period = centsScale.intervalCents.getLast();

	int size = tuning->getVirtualSize();
	if (size == 0)
		size = centsScale.intervalCents.size();

	degreeAngles.clear();
	for (int i = 0; i < size; i++)
	{
		auto angle = scaleDegreeToAngle(i, period);
		degreeAngles.add(angle);
	}

	resized();
}

void ToneCircle::setScale(const TuningTableBase* tuningIn)
{
	tuning = tuningIn;

	// User functional definition if we can
	auto functional = dynamic_cast<const FunctionalTuning*>(tuningIn);
	if (functional)
	{
		return setScale(functional);
	}

	auto definition = CentsDefinition::ExtractFromTuningTable(tuning);
	updateScale(definition);
}

void ToneCircle::setScale(const FunctionalTuning* tuningIn)
{
	tuning = tuningIn;
	
	auto definition = tuningIn->getDefinition();
	updateScale(definition);
}

void ToneCircle::setScale(const CentsDefinition& centsScale)
{
	tuning = nullptr;
	updateScale(centsScale);
}


float ToneCircle::scaleDegreeToAngle(int scaleDegreeIn, double period) const
{
	return 2.0f * juce::float_Pi * tuning->centsFromRoot(scaleDegreeIn) / 1200;
}

void ToneCircle::mouseMove(const juce::MouseEvent& e)
{
	int nearDegree = -1;
	double shortestDistance = 10e10;
	for (int i = 0; i < degreePoints.size(); i++)
	{
		auto point = degreePoints[i];
		auto distance = e.position.getDistanceFrom(point);
		if (distance <= nearDistanceThreshold && distance < shortestDistance)
		{
			shortestDistance = distance;
			nearDegree = i;
		}
	}

	mouseNearDegree = nearDegree;

	if (nearDegree >= 0)
	{
		auto text = juce::String(tuning->centsFromRoot(nearDegree)) + " cents";
		intervalLabel->setText(text, juce::NotificationType::dontSendNotification);
		intervalLabel->setVisible(true);
		return;
	}
	
	intervalLabel->setVisible(false);
}