/*
  ==============================================================================

    TuningBase.h
    Created: 22 Dec 2021 10:44:38am
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "TuningMath.h"

class TuningBase
{
protected:
	juce::String name;
	juce::String description;

	int rootIndex;
	double rootFrequency;

public:

	TuningBase(juce::String nameIn = juce::String(), juce::String descriptionIn = juce::String())
		: name(nameIn), description(descriptionIn) {}

	TuningBase(double rootFrequencyIn, juce::String nameIn = juce::String(), juce::String descriptionIn = juce::String())
		: rootFrequency(rootFrequencyIn), name(nameIn), description(descriptionIn) {}

	TuningBase(int rootIndexIn, double rootFrequencyIn, juce::String nameIn = juce::String(), juce::String descriptionIn = juce::String())
		: rootIndex(rootIndexIn), rootFrequency(rootFrequencyIn), name(nameIn), description(descriptionIn) {}

	virtual juce::String getName() const { return name; }
	virtual juce::String getDescription() const { return description; }

	virtual int getRootIndex() const { return rootIndex; }
	virtual double getRootFrequency() const { return rootFrequency; }
	virtual int getTuningSize() const = 0;

	virtual void setName(juce::String nameIn) { name = nameIn; };
	virtual void setDescription(juce::String descIn) { description = descIn; }

	virtual void setRootFrequency(double frequency) = 0;

	virtual double centsAt(int index) const = 0;
	virtual double centsFromRoot(int steps) const { return centsAt(steps - rootIndex); }

	virtual double semitonesAt(int index) const { return centsAt(index) * 0.01; }
	virtual double semitonesFromRoot(int steps) const { return centsFromRoot(steps) * 0.01; }

	virtual double frequencyAt(int index) const = 0;
	virtual double frequencyFromRoot(int steps) const { return frequencyAt(steps - rootIndex); }

	virtual double mtsAt(int index) const 
	{ 
		return frequencyToMTS(frequencyAt(index));
	}
	virtual double mtsFromRoot(int steps) { return mtsAt(steps - rootIndex); }

	virtual int closestIndexToFrequency(double frequency) const = 0;
	virtual int closestFrequency(double frequency)
	{
		auto index = closestIndexToFrequency(frequency);
		return frequencyAt(index);
	}

	virtual int closestIndexToCents(double centsFromRoot) const
	{
		double frequency = rootFrequency * centsToRatio(centsFromRoot);
		return closestIndexToFrequency(frequency);
	}
	virtual int closestCents(double centsFromRoot) const
	{
		auto index = closestIndexToCents(centsFromRoot);
		return centsAt(index);
	}

	virtual int closestIndexToSemitones(double semitonesFromRoot) const { return closestIndexToCents(semitonesFromRoot * 100.0); }
	virtual int closestSemitones(double semitonesFromRoot)
	{
		auto index = closestIndexToSemitones(semitonesFromRoot);
		return semitonesAt(index);
	}

	virtual int closestIndexToMts(double mts) const
	{
		double frequency = mtsToFrequency(mts);
		return closestIndexToFrequency(frequency);
	}
	virtual double closestMts(double mts) const
	{
		auto index = closestIndexToMts(mts);
		return mtsAt(index);
	}
};
