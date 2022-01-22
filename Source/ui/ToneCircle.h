/*
  ==============================================================================

    ToneCircle.h
    Created: 5 Aug 2020 11:14:32pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../tuning/MappedTuning.h"

//==============================================================================
/*
*/
class ToneCircle : public juce::Component
{
public:

	enum ColourIds
	{
		circleOutlineColourId = 0x00100100,
		intervalStartColourId = 0x00200100,
		intervalMiddleColourId = 0x00200101,
		intervalEndColourId = 0x00200102
	};

private:
	// Optional pointer to last set tuning
	const TuningTableBase* tuning;

	// Displayed tuning
	CentsDefinition centsScale;

	juce::Array<float> degreeAngles;
	juce::Array<juce::Point<float>> degreePoints;
	juce::ColourGradient intervalTextGradient;

	std::unique_ptr<juce::Label> intervalLabel;

	double nearDistanceThreshold = 15;
	int mouseNearDegree = -1;

	// Drawing helpers
	juce::Point<float> center;

	float margin = 1.0f / 8.0f;
	float circleRadius;
	juce::Rectangle<float> circleBounds;

	float noteMarkerRadius = 6.0f;

private:

	void updateScale(const CentsDefinition& scaleToCopy);

	float scaleDegreeToAngle(int scaleDegreeIn, double period) const;


public:
    ToneCircle(juce::String componentName);
    ~ToneCircle() override;

    void paint (juce::Graphics&) override;
    void resized() override;

	void mouseMove(const juce::MouseEvent&) override;

	void setScale(const TuningTableBase* tuningIn);
	void setScale(const FunctionalTuning* tuningIn);
	void setScale(const CentsDefinition& scaleToCopy);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToneCircle)
};
