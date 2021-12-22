/*
  ==============================================================================

    TuningBase.h
    Created: 22 Dec 2021 10:44:38am
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class TuningBase
{
protected:
	juce::String name;
	juce::String description;

public:
	
	TuningBase(juce::String nameIn = juce::String(), juce::String descriptionIn = juce::String()) 
		: name(nameIn), description(descriptionIn) {}

	virtual juce::String getName() const { return name; }
	virtual juce::String getDescription() const { return description; }

	virtual int getRootIndex() const = 0;
	virtual double getRootFrequency() const = 0;
	virtual int getTuningSize() const = 0;


	virtual void setName(juce::String nameIn) { name = nameIn; };
	virtual void setDescription(juce::String descIn) { description = descIn; }

	virtual double getNoteInCents(int noteNumber, int channel = 1) const = 0;
	virtual double getNoteInSemitones(int noteNumber, int channel = 1) const = 0;

	virtual double getNoteFrequency(int noteNumber, int channel = 1) const = 0;
	virtual double getNoteInMTS(int noteNumber, int channel = 1) const = 0;

	virtual int closestNoteIndex(double mtsIn) const = 0;
	virtual int closestMtsNote(double mtsIn) const = 0;

};
