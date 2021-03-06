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

class TuningTableBase : public TuningBase
{
public:

	TuningTableBase(int rootIndexIn, double rootFrequencyIn, juce::String nameIn = juce::String(), juce::String descriptionIn = juce::String())
		: TuningBase(rootIndexIn, rootFrequencyIn, nameIn, descriptionIn) {}

	virtual int getTableSize() const = 0;

	virtual juce::String getPeriodString() const { return juce::String(); }
	virtual juce::String getSizeString() const { return juce::String(); }

	virtual double getVirtualPeriod() const { return 0.0; }
	virtual double getVirtualSize() const { return 0.0; }

	virtual juce::Array<double> getFrequencyTable() const = 0;
	virtual juce::Array<double> getMtsTable() const = 0;
};

class TuningTable : public TuningTableBase
{
public:

	struct Definition
	{
		juce::Array<double> frequencies;
		int rootIndex = -1;
		juce::String name;
		juce::String description;
		juce::String periodString;
		double virtualPeriod = 0;
		double virtualSize = 0;

		bool operator==(const Definition& def)
		{
			return frequencies == def.frequencies
				&& rootIndex == def.rootIndex
				&& name == def.name
				&& description == def.description
				&& periodString == def.periodString
				&& virtualPeriod == def.virtualPeriod
				&& virtualSize == virtualSize;
		}
	};

private:

	// Parameters	
	juce::Array<double> frequencyTable;
	int tableSize; // Derived classes can set this separately

	// In case the file can provide some context for the tuning table
	juce::String periodString;
	double virtualPeriod = 0;
	double virtualSize = 0;

	// Metadata
    juce::Array<double> mtsTable;
	double rootMts;

private:

	void refreshTableMetadata();

protected:

	void setVirtualPeriod(double period, juce::String periodStr = "");

	void setVirtualSize(double size);

	void setTableWithFrequencies(juce::Array<double> frequencies, int newRootIndex = -1);

	void setTableWithMts(juce::Array<double> mts, int newRootIndex = -1);

	void transposeTableByRatio(double ratio);

public:

	TuningTable(Definition definition);

    TuningTable(const TuningTable&);

	virtual bool operator==(const TuningTable&);
	virtual bool operator!=(const TuningTable&);

	TuningTable::Definition getDefinition() const;

	// TuningTableBase implementation

	virtual int getTableSize() const override;
	
	virtual juce::String getPeriodString() const override;
	virtual juce::String getSizeString() const override;

	virtual double getVirtualPeriod() const override;
	virtual double getVirtualSize() const override;

	virtual double getRootMts() const override;

	virtual juce::Array<double> getFrequencyTable() const override;
	virtual juce::Array<double> getMtsTable() const override;
	//virtual juce::Array<MTSTriplet> getMTSDataTable() const; // this should have some parameters, or maybe even be static

	//virtual juce::Array<double> getIntervalCentsList() const override;
	//virtual juce::Array<double> getIntervalRatioList() const;


	// TuningBase implementation

	virtual void setRootFrequency(double frequency) override;

	virtual double centsAt(int index) const override;
	virtual double frequencyAt(int index) const override;
	virtual double mtsAt(int index) const override;

	virtual int closestIndexToFrequency(double frequency) const override;
	virtual int closestIndexToCents(double centsFromRoot) const override;

protected:

	void setTableSize(int tableSizeIn) { tableSize = tableSizeIn; }

	static juce::Array<double> frequencyToMtsTable(juce::Array<double> frequenciesIn);

	static juce::Array<double> mtsToFrequencyTable(juce::Array<double> mtsIn);

};
