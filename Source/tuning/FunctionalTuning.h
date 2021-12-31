/*
  ==============================================================================

    FunctionalTuning.h
    Created: 29 Dec 2021 10:27:23pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "./CentsDefinition.h"
#include "./TuningTable.h"

class FunctionalTuning : public TuningTable
{
    Map<double> centsMap;
    bool tablesAreBuilt = false;

    int tuningSize;

    double periodCents;
    double periodRatio;

private:

    void setupCentsMap(const juce::Array<double>& cents);

    static TuningTable::Definition setupEmptyTableDefinition(const CentsDefinition& definition);

public:

    /*
        Expects a full interval table in cents, ending with period. May or may not include unison.
    */
    FunctionalTuning(CentsDefinition definition = CentsDefinition(), bool buildTables = false);

    FunctionalTuning(const FunctionalTuning&);
    
    virtual bool operator==(const FunctionalTuning&);
    virtual bool operator!=(const FunctionalTuning&);

    virtual CentsDefinition getDefinition() const;

    virtual juce::Array<double> getIntervalCentsList() const;
    virtual juce::Array<double> getIntervalRatioList() const;

    virtual double getPeriodCents() const;
    virtual double getPeriodSemitones() const;
    virtual double getPeriodRatio() const;

    virtual double calculateFrequencyFromRoot(int stepsFromRoot) const;
    virtual double calculateCentsFromRoot(int stepsFromRoot) const;
    virtual double calculateSemitonesFromRoot(int stepsFromRoot) const;
    virtual double calculateMtsFromRoot(int stepsFromRoot) const;

    // TuningBase implementation
    
    virtual int getTuningSize() const { return tuningSize; }


    // TuningTable re-implementation

    virtual void setRootFrequency(double frequency) override;

    virtual int getTableSize() const override;
    virtual int getTableSize(bool calculate) const;

    virtual juce::Array<double> getFrequencyTable() const override;
    virtual juce::Array<double> getMtsTable() const override;

    virtual double centsAt(int index) const override;
    virtual double frequencyAt(int index) const override;
    virtual double mtsAt(int index) const override;

    virtual int closestIndexToFrequency(double frequency) const override;
    virtual int closestIndexToCents(double centsFromRoot) const override;

private:

    virtual juce::Array<double> buildFrequencyTable(int size = 0) const;

protected:

    //static void calculateRootAndTableSizeFromMap(const Map<double>& centsMap, double rootFrequency, int& rootIndex, int& tableSize);

public:

    static CentsDefinition StandardTuningDefinition()
    {
    	return CentsDefinition::CentsDivisions(12);
    }

    static std::shared_ptr<FunctionalTuning> StandardTuning()
    {
        auto definition = StandardTuningDefinition();
    	return std::make_shared<FunctionalTuning>(definition);
    }
};
