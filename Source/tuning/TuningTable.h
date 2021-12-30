/*
  ==============================================================================

    TuningTable.h
    Created: 20 November 2021 1:00pm
    Author:  Vincenzo Sicurella

	A TuningTable with a lookup table that maps itself with the MTS range.

  ==============================================================================
*/

#pragma once

#include "TuningBase.h"
#include "../mapping/Map.h"
#include "CentsDefinition.h"

class TuningTable : public TuningBase
{
	// Parameters	
	juce::Array<double> centsTable;

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

	double rootMts;


	// Debug
	double dbgFreqTable[2048];
	double dbgMtsTable[2048];

private:

	void setupCentsMap(const juce::Array<double>& cents);

	void setupRootAndTableSize();

	void rebuildTables();

public:

	/*
		Expects a full interval table in cents, ending with period. May or may not include unison.
	*/
	TuningTable(CentsDefinition definition=CentsDefinition());

    TuningTable(const TuningTable&);

	// TuningBase implementation

	virtual int getTuningSize() const override { return tuningSize; }

	virtual void setRootFrequency(double frequency) override;

	virtual double centsAt(int index) const override;
	virtual double frequencyAt(int index) const override;
	virtual double mtsAt(int index) const override;

	virtual int closestIndexToFrequency(double frequency) const override;
	virtual int closestIndexToFrequency(double frequency, bool useLookup) const;

	virtual int closestIndexToCents(double centsFromRoot) const override;
	
	// TuningTable declarations 

	virtual bool operator==(const TuningTable&);
	virtual bool operator!=(const TuningTable&);

	virtual CentsDefinition getDefinition() const;

	virtual int getTuningTableSize() const;

	virtual double getVirtualPeriod() const;
	virtual double getVirtualSize() const;

	virtual double getPeriodCents() const;
	virtual double getPeriodSemitones() const;
	virtual double getPeriodRatio() const;

	virtual double getRootMts() const;

	virtual int getScaleDegree(int index) const;

	virtual juce::Array<double> getIntervalCentsList() const override;
	virtual juce::Array<double> getIntervalRatioList() const;

	virtual juce::Array<double> getFrequencyTable() const;
	virtual juce::Array<double> getMtsTable() const;
	//virtual juce::Array<MTSTriplet> getMTSDataTable() const; // this should have some parameters, or maybe even be static

	virtual double calculateFrequencyFromRoot(int stepsFromRoot) const;
	virtual double calculateCentsFromRoot(int stepsFromRoot) const;
	virtual double calculateSemitonesFromRoot(int stepsFromRoot) const;
	virtual double calculateMtsFromRoot(int stepsFromRoot) const;

public:

	static CentsDefinition StandardTuningDefinition()
	{
		return CentsDefinition::CentsDivisions(12);
	}

	static std::unique_ptr<TuningTable> StandardTuning()
	{
		return std::make_unique<TuningTable>(StandardTuningDefinition());
	}
};
