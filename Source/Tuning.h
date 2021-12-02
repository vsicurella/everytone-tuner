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

#define TUNING_TABLE_SIZE 2048

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

	int midiIndex(int midiNote, int midiChannel) const { return modulo(midiChannel << 7 + midiNote, TUNING_TABLE_SIZE); }

	template<typename U>
	juce::String tableToString(const juce::Array<U>& table, int startChannel, int endChannel) const;

public:

	struct Reference
	{
		int rootMidiNote = 60;
		int rootMidiChannel = 1;
		double rootFrequency = 261.6255653;
	};

	struct Definition
	{
		Reference reference;
		juce::String name = "";
		juce::String description = "";
	};

	struct IntervalDefinition : Definition
	{
		juce::Array<double> intervalCents = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200 };

        IntervalDefinition() {}
	};

public:

	/*
		Expects a full interval table in cents, ending with period. May or may not include unison.
	*/
	Tuning(IntervalDefinition definition=IntervalDefinition());

    Tuning(const Tuning&);
    
	virtual int getRootIndex() const { return midiIndex(rootMidiNote, rootMidiChannel); }
	virtual int getRootMidiNote() const { return rootMidiNote; }
	virtual int getRootMidiChannel() const { return rootMidiChannel; }

	virtual double getRootFrequency() const { return rootFrequency; }
	virtual Reference getReference() const;

	virtual int getTuningSize() const { return tuningSize; }

	virtual juce::String getName() const { return name; }
	virtual juce::String getDescription() const { return description; }
	
	virtual Definition getDefinition() const;
	
	
	virtual void setName(juce::String nameIn);
	virtual void setDescription(juce::String descIn);
	
	virtual void setFrequency(double frequency);
	virtual void setRootMidiNote(int midiNote);
	virtual void setRootMidiChannel(int rootMidiChannel);
	virtual void setReference(Reference reference);
	
	virtual double getPeriodCents() const;
	virtual double getPeriodSemitones() const;
	
    virtual double getNoteInCents(int noteNumber, int tableIndex = 0) const;
    virtual double getNoteInSemitones(int noteNumber, int tableIndex = 0) const;

	//virtual double getNoteInMTS(int noteNumber, int tableIndex = 0) const;


	virtual juce::String centsTableToString(int startMidiChannel, int endMidiChannel) const;
	virtual juce::String semitoneTableToString(int startMidiChannel, int endMidiChannel) const;
	virtual juce::String mtsTableToString(int startMidiChannel, int endMidiChannel) const;


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
