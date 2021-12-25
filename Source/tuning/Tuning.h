/*
  ==============================================================================

    Tuning.h
    Created: 20 November 2021 1:00pm
    Author:  Vincenzo Sicurella

	A Tuning with a lookup table that maps itself with the MTS range.

  ==============================================================================
*/

#pragma once

#include "TuningBase.h"
#include "../mapping/Map.h"
#include "CentsDefinition.h"

class Tuning : public TuningBase
{
	// Parameters	
	juce::Array<double> centsTable;
	double rootFrequency;

	int lookupTableSize;

	// Mainly for equal/rank-1 temperaments to record the divided period
	double virtualPeriod = 0;
	double virtualSize = 0;

	// Metadata
	int tuningSize;
	std::unique_ptr<Map<double>> centsMap;

	juce::Array<double> ratioTable;
    juce::Array<double> frequencyTable;
    juce::Array<double> mtsTable;

	double periodCents;
	double periodRatio;
	//double periodMts;

	double rootMts;


	// Debug
	double dbgFreqTable[2048];
	double dbgMtsTable[2048];

private:

	void setupCentsMap(const juce::Array<double>& cents);

	void setupRootAndTableSize();

	void rebuildTables();

public:

	//struct Reference
	//{
	//	int rootMidiNote = 60;
	//	int rootMidiChannel = 1;
	//	double rootFrequency = 261.6255653;

	//	bool operator==(const Reference& reference)
	//	{
	//		return rootMidiNote == reference.rootMidiNote
	//			&& rootMidiChannel == reference.rootMidiChannel
	//			&& rootFrequency == reference.rootFrequency;
	//	}

 //       juce::String toString() const 
 //       {
 //           juce::StringArray arr =
 //           {
 //               "note: " + juce::String(rootMidiNote),
 //               "channel: " + juce::String(rootMidiChannel),
 //               "frequency: " + juce::String(rootFrequency)
 //           };
 //           
 //           return "{ " + arr.joinIntoString(", ") + " }";
 //       }
	//};

public:

	/*
		Expects a full interval table in cents, ending with period. May or may not include unison.
	*/
	Tuning(CentsDefinition definition=CentsDefinition());

    Tuning(const Tuning&);

	// TuningBase implementation

	virtual int getTuningSize() const override { return tuningSize; }

	virtual void setRootFrequency(double frequency) override;

	virtual double centsAt(int index) const override;
	virtual double frequencyAt(int index) const override;
	virtual double mtsAt(int index) const override;

	virtual int closestIndexToFrequency(double frequency) const override;
	virtual int closestIndexToFrequency(double frequency, bool useLookup) const;

	virtual int closestIndexToCents(double centsFromRoot) const override;
	
	// Tuning declarations 

	virtual bool operator==(const Tuning&);
	virtual bool operator!=(const Tuning&);

	virtual CentsDefinition getDefinition() const;

	virtual double getVirtualPeriod() const;
	virtual double getVirtualSize() const;

	virtual double getPeriodCents() const;
	virtual double getPeriodSemitones() const;
	virtual double getPeriodRatio() const;

	virtual double getRootMts() const;

	virtual int getScaleDegree(int index) const;

	virtual juce::Array<double> getIntervalCentsTable() const;
	virtual juce::Array<double> getIntervalRatioTable() const;

	virtual juce::Array<double> getFrequencyTable() const;
	virtual juce::Array<double> getMtsTable() const;
	//virtual juce::Array<MTSTriplet> getMTSDataTable() const; // this should have some parameters, or maybe even be static

	virtual double calculateFrequencyFromRoot(int stepsFromRoot) const;
	virtual double calculateCentsFromRoot(int stepsFromRoot) const;
	virtual double calculateSemitonesFromRoot(int stepsFromRoot) const;
	virtual double calculateMtsFromRoot(int stepsFromRoot) const;

public:

	static Tuning StandardTuning()
	{
		auto definition = CentsDefinition::CentsDivisions(12);
		return Tuning(definition);
	}
};
