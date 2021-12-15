/*
  ==============================================================================

    Tuning.h
    Created: 20 November 2021 1:00pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#pragma once

#include "TuningMath.h"
#include "./mapping/Map.h"

#define TUNING_TABLE_SIZE 2048

class Tuning
{
    std::unique_ptr<Map<double>> tuningMap;
	
	juce::Array<double> centsTable;
	juce::Array<double> ratioTable;

    juce::Array<double> frequencyTable;
    juce::Array<double> mtsTable;

	double dbgFreq[TUNING_TABLE_SIZE];
	double dbgMts[TUNING_TABLE_SIZE];

	double periodCents;
	double periodRatio;
	double periodMts;

	double virtualPeriod; // Mainly for equal/rank-1 temperaments to record the divided period

	int tuningSize;

    int rootMidiNote;
	int rootMidiChannelIndex;
	double rootFrequency;
	double rootMts;

    int transpose;

	juce::String name;
	juce::String description;

private:

	void setupTuning(const juce::Array<double>& cents);

	void rebuildTables();

	static int midiIndex(int midiNote, int midiChannelIndex) { return modulo((midiChannelIndex << 7) + midiNote, TUNING_TABLE_SIZE); }
    int rootMidiIndex() const { return midiIndex(rootMidiNote, rootMidiChannelIndex); }

	template<typename U>
	juce::String tableToString(const juce::Array<U>& table, int startChannel, int endChannel) const;

public:

	struct Reference
	{
		int rootMidiNote = 60;
		int rootMidiChannel = 1;
		double rootFrequency = 261.6255653;

        juce::String toString() const 
        {
            juce::StringArray arr =
            {
                "note: " + juce::String(rootMidiNote),
                "channel: " + juce::String(rootMidiChannel),
                "frequency: " + juce::String(rootFrequency)
            };
            
            return "{ " + arr.joinIntoString(", ") + " }";
        }
	};

	struct Definition
	{
		Reference reference;
        int transpose = 0;
		juce::String name = "";
		juce::String description = "";

        juce::String toString() const 
        {
            juce::StringArray arr =
            {
                "name: " + name,
                "description: " + description,
                "note: " + juce::String(reference.rootMidiNote),
                "channel: " + juce::String(reference.rootMidiChannel),
                "frequency: " + juce::String(reference.rootFrequency),
                "transpose: " + juce::String(transpose),
            };
            
            return "{ " + arr.joinIntoString(", ") + " }";
        }
	};

	struct CentsDefinition : Definition
	{
		juce::Array<double> intervalCents = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200 };

		double virtualPeriod = 0;

        CentsDefinition() {}
		CentsDefinition(const juce::Array<double> centsIn) : intervalCents(centsIn) {}
		CentsDefinition(double divisions, double periodCents = 1200.0)
		{
			double step = periodCents / divisions;
			intervalCents = { step };
			name = juce::String(divisions) + "ed " + juce::String(periodCents) + " cents";
		}

		static CentsDefinition RatioDivisions(double divisions, double periodRatio = 2.0)
		{
			auto cents = ratioToCents(periodRatio);
			auto definition = CentsDefinition(divisions, cents);
			return definition;
		}

		void operator=(const Tuning::Definition& definition)
		{
			reference = definition.reference;
			name = definition.name;
			description = definition.description;
			transpose = definition.transpose;
		}

		void operator=(const Tuning::CentsDefinition& definition)
		{
			intervalCents = definition.intervalCents;
			*this = (Definition)definition;
		}
	};

public:

	/*
		Expects a full interval table in cents, ending with period. May or may not include unison.
	*/
	Tuning(CentsDefinition definition=CentsDefinition());

    Tuning(const Tuning&);
    
	virtual int getRootIndex() const { return midiIndex(rootMidiNote, rootMidiChannelIndex); }
	virtual int getRootMidiNote() const { return rootMidiNote; }
	virtual int getRootMidiChannel() const { return rootMidiChannelIndex + 1; }

	virtual double getRootFrequency() const { return rootFrequency; }
	virtual Reference getReference() const;

	virtual int getTuningSize() const { return tuningSize; }

	virtual juce::String getName() const { return name; }
	virtual juce::String getDescription() const { return description; }
	
	virtual Definition getDefinition() const;
	
	
	virtual void setName(juce::String nameIn);
	virtual void setDescription(juce::String descIn);
	
	virtual void setRootFrequency(double frequency);
	virtual void setRootMidiNote(int midiNote);
	virtual void setRootMidiChannel(int rootMidiChannel);
	virtual void setReference(Reference reference);
	
	virtual double getPeriodCents() const;
	virtual double getPeriodSemitones() const;
	
    virtual double getNoteInCents(int noteNumber, int channel = 1) const;
    virtual double getNoteInSemitones(int noteNumber, int channel = 1) const;

	virtual double getNoteFrequency(int noteNumber, int channel = 1) const;
	virtual double frequencyTableAt(int tableIndex) const;

	virtual double getNoteInMTS(int noteNumber, int channel = 1) const;
	virtual double mtsTableAt(int tableIndex) const;
 
    virtual juce::Array<double> getIntervalCentsTable() const;
    virtual juce::Array<double> getIntervalRatioTable() const;

    virtual juce::Array<double> getFrequencyTable() const;
    virtual juce::Array<double> getMTSTable() const;
    virtual juce::Array<MTSTriplet> getMTSDataTable() const;

	virtual juce::String mtsTableToString(int startMidiChannel, int endMidiChannel) const;


	/*
		Returns the period-reduced scale degree of the note number passed in
	*/
	int getScaleDegree(int noteNumber) const;

	/*
		Returns the closest MTS note to the one passed in
	*/
	int closestNoteIndex(double mtsIn) const;
	int closestMtsNote(double mtsIn) const;

public:

	static Tuning StandardTuning()
	{
		CentsDefinition definition;
		definition.name = "12-edo";
		definition.description = "Octave divided in 12 equal steps";
		return Tuning(definition);
	}
};
