/*
  ==============================================================================

    Tuning.h
    Created: 20 November 2021 1:00pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#pragma once

#include "TuningMath.h"
#include "../../Keytographer/Source/Map.h"

class Tuning
{
    std::unique_ptr<Keytographer::Map<double>> tuningMap;

    void setupTuning(const juce::Array<double>& cents);

protected:

	double periodCents;
	double periodSemitones;

	int tuningSize;
    int rootMidiNote; // note tuning is centered on

	juce::String name;
	juce::String description;

public:

	/*
		Expects a full interval table in cents, ending with period. May or may not include unison.
	*/
	Tuning(const juce::Array<double>& intervalCentsIn, int rootMidiNoteIn = 60, juce::String nameIn = "", juce::String descriptionIn = "");

    Tuning(const Tuning&);

	void setRootNote(int rootNoteIn);
	void setDescription(juce::String descIn);

    int getTuningSize() const;

	double getPeriodCents() const;
	double getPeriodSemitones() const;
	
    juce::Array<double> getIntervalCents() const;
    juce::Array<double> getIntervalSemitones() const;
    
    virtual double getNoteInSemitones(int noteNumber) const;
    virtual double getNoteInCents(int noteNumber) const;
    
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

    static Tuning StandardTuning(int rootNote=60)
    {
    juce::Array<double> table;
        for (int i = 100; i <= 1200; i+=100)
            table.add(i);

        return Tuning(table, 60, "12edo");
    }
};
