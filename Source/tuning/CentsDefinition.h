/*
  ==============================================================================

    CentsDefinition.h
    Created: 24 Dec 2021 10:11:23pm
    Author:  Vincenzo

	Tuning definition based on a list of cents values

	TODO remove JUCE dependency

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "TuningMath.h"

struct CentsDefinition
{
	juce::Array<double> intervalCents = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200 };

	double rootFrequency = 440.0;

	juce::String name;
	juce::String description;

	double virtualPeriod = 0;
	double virtualSize = 0;

	static juce::String getEqualDivisionsNameAndDescription(juce::String& name, juce::String& description, double divisions, double period, bool isCents = false)
	{
		bool integerDivisions = (int)divisions == divisions;
		auto strDivisions = (integerDivisions)
			? juce::String((int)divisions)
			: juce::String(divisions);

		name = strDivisions + "-ed";

		if ((period == 1200.0 && isCents) || (period == 2.0 && !isCents))
		{
			name += "o";
			description = "The octave";
		}
		else
		{
			if (isCents)
			{
				description = juce::String(period) + " cents";
				name += description;
			}
			else
			{
				bool integerPeriod = (int)period == period;
				auto strPeriod = (integerPeriod)
					? juce::String((int)period)
					: juce::String(period);

				name += strPeriod;
				description = strPeriod;
			}
		}

		description += " divided into " + strDivisions + " equal steps.";
	}

	bool operator==(const CentsDefinition& d) const
	{
		return intervalCents == d.intervalCents
			&& rootFrequency == d.rootFrequency
			&& name == d.name
			&& description == d.description;
	}
	bool operator!=(const CentsDefinition& d) const { return !operator==(d); }

	static CentsDefinition CentsDivisions(double divisions, double periodCents = 1200.0, double frequency = 440.0)
	{
		double step = periodCents / divisions;

		juce::String name, description;
		getEqualDivisionsNameAndDescription(name, description, divisions, periodCents, true);

		CentsDefinition definition = { { step }, frequency, name, description, periodCents, divisions };
		return definition;
	}

	static CentsDefinition RatioDivisions(double divisions, double periodRatio = 2.0, double frequency = 440.0)
	{
		double step = ratioToCents(periodRatio) / divisions;

		juce::String name, description;
		getEqualDivisionsNameAndDescription(name, description, divisions, periodRatio);

		CentsDefinition definition = { { step }, frequency, name, description, periodRatio, divisions };
		return definition;
	}
};