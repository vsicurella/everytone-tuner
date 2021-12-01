/*
  ==============================================================================

    Tuning.h
    Created: 20 November 2021 1:00pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#pragma once

#include "TuningMath.h"
#include "Map.h"

class Tuning
{
    std::unique_ptr<Keytographer::Map<double>> tuningMap;
	
	juce::Array<double> centsTable;

	double periodCents;
	int tuningSize;

    int rootMidiNote;
	int rootMidiChannel;
	double rootFrequency;

	juce::String name;
	juce::String description;

private:

	void setupTuning(const juce::Array<double>& cents);

	void rebuildTable();

public:

	struct Definition
	{
		juce::String name = "";
		juce::String description = "";
	};

	struct IntervalDefinition : Definition
	{
		juce::Array<double> intervalCents = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200 };
		int rootMidiNote = 60;
		int rootMidiChannel = 1;
		double rootFrequency = 261.6255653;
	};

public:

	/*
		Expects a full interval table in cents, ending with period. May or may not include unison.
	*/
	Tuning(IntervalDefinition definition=IntervalDefinition());

    Tuning(const Tuning&);

	void setRootNote(int rootNoteIn);
	void setDescription(juce::String descIn);

    int getTuningSize() const;

	double getPeriodCents() const;
	double getPeriodSemitones() const;
	
    virtual double getNoteInCents(int noteNumber, int tableIndex = 0) const;
    virtual double getNoteInSemitones(int noteNumber, int tableIndex = 0) const;

	//virtual double getNoteInMTS(int noteNumber, int tableIndex = 0) const;
    
    int getRootNote() const;

	juce::String getName() const;
	juce::String getDescription() const;
	juce::String toString() const;

	/*
		Returns the period-reduced scale degree of the note number passed in
	*/
	int getScaleDegree(int noteNumber) const;

	/*
		Returns the closest note number to the cents value passed in, 
		from reference of the root note
	*/
	int closestNoteToCents(double cents) const;

public:

	static Tuning StandardTuning()
	{
		IntervalDefinition definition;
		definition.name = "12-edo";
		definition.description = "Octave divided in 12 equal steps";
		return Tuning(definition);
	}
};
