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

public:
    ToneCircle(juce::String componentName);
    ~ToneCircle() override;

    void paint (juce::Graphics&) override;
    void resized() override;

	void setTuning(const MappedTuningTable* tuningIn);

//==============================================================================
private:

	float scaleDegreeToAngle(int scaleDegreeIn, double period) const;

//==============================================================================

private:

	const MappedTuningTable* tuning;

	juce::Array<float> degreeAngles;
	juce::Array<juce::Point<float>> degreePoints;
	juce::ColourGradient intervalTextGradient;

	// Drawing helpers
	juce::Point<float> center;
	
	float margin = 1.0f / 8.0f;
	float circleRadius;
	juce::Rectangle<float> circleBounds;

	float noteMarkerRadius = 6.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToneCircle)
};
