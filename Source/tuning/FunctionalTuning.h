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
    juce::Array<double> centsTable;

    bool tablesAreBuilt = false;

    // Metadata
    std::unique_ptr<Map<double>> centsMap;

    int tuningSize;

    juce::Array<double> ratioTable;

    double periodCents;
    double periodRatio;


private:

    void setupCentsMap(const juce::Array<double>& cents);

    void setupRootAndTableSize();

    void rebuildTables();

public:

    /*
        Expects a full interval table in cents, ending with period. May or may not include unison.
    */
    FunctionalTuning(CentsDefinition definition = CentsDefinition());
    
    virtual bool operator==(const FunctionalTuning&);
    virtual bool operator!=(const FunctionalTuning&);

    virtual CentsDefinition getDefinition() const;

    virtual double getPeriodCents() const;
    virtual double getPeriodSemitones() const;
    virtual double getPeriodRatio() const;

    virtual double calculateFrequencyFromRoot(int stepsFromRoot) const;
    virtual double calculateCentsFromRoot(int stepsFromRoot) const;
    virtual double calculateSemitonesFromRoot(int stepsFromRoot) const;
    virtual double calculateMtsFromRoot(int stepsFromRoot) const;


    // TuningBase implementation
    
    virtual int getTuningSize() const override { return tuningSize; }


    // TuningTable re-implementation

    virtual double centsAt(int index) const override;
    virtual double frequencyAt(int index) const override;
    virtual double mtsAt(int index) const override;

    virtual int closestIndexToFrequency(double frequency) const override;
    virtual int closestIndexToFrequency(double frequency, bool useLookup) const;

    virtual int closestIndexToCents(double centsFromRoot) const override;

    virtual int getTuningTableSize() const;

    virtual juce::Array<double> getIntervalCentsList() const override;
    virtual juce::Array<double> getIntervalRatioList() const override;

    virtual juce::Array<double> getFrequencyTable() const override;
    virtual juce::Array<double> getMtsTable() const override;
    //virtual juce::Array<MTSTriplet> getMTSDataTable() const; // this should have some parameters, or maybe even be static

};
