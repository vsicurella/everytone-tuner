/*
  ==============================================================================

    FunctionalTuning.cpp
    Created: 29 Dec 2021 10:27:23pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "FunctionalTuning.h"

FunctionalTuning::FunctionalTuning(CentsDefinition definition)
    : tuningSize(definition.intervalCents.size()),
    TuningBase(definition.rootFrequency, definition.name, definition.description)
{
    setVirtualPeriod(definition.virtualPeriod, juce::String(definition.virtualPeriod) + " cents");
    setupCentsMap(definition.intervalCents);
}

void FunctionalTuning::setupCentsMap(const juce::Array<double>& cents)
{
    centsTable = cents;

    periodCents = cents.getLast();
    periodRatio = centsToRatio(periodCents);

    auto tuningShifted = juce::Array<double>(cents);
    tuningShifted.remove(tuningShifted.getLast());
    tuningShifted.insert(0, 0);

    Map<double>::Definition definition =
    {
        tuningSize,
        tuningShifted.data(),
        periodCents,
        0,                     /* pattern rootIndex */
        0, /* tuning index rootIndex */
    };

    centsMap.reset(new Map<double>(definition));
}


void FunctionalTuning::setupRootAndTableSize()
{
    double lowestRatio = MTS_LOWEST_FREQ / rootFrequency;
    double lowestCents = ratioToCents(lowestRatio);
    int lowestFromRoot = centsMap->closestIndexTo(lowestCents);

    double highestRatio = MTS_HIGHEST_FREQ / rootFrequency;
    double highestCents = ratioToCents(highestRatio);
    int highestFromRoot = centsMap->closestIndexTo(highestCents);

    rootIndex = -lowestFromRoot;
    lookupTableSize = highestFromRoot - lowestFromRoot + 1;

    //juce::Logger::writeToLog("Best mapping, rootIndex: " + juce::String(lowestToNewRoot) + ", size: " + juce::String(tableSize));
}

void TuningTable::rebuildTables()
{
    setupRootAndTableSize();

    // Build ratio table
    double cents, ratio;
    for (int i = 0; i < tuningSize; i++)
    {
        cents = centsMap->at(i);
        ratio = centsToRatio(cents);
        ratioTable.set(i, ratio);
    }

    double periodRatio = centsToRatio(periodCents);
    rootMts = roundN(10, frequencyToMTS(rootFrequency));

    // Build Frequency and MTS freq tables
    int offset;
    double intervalRatio, frequency, size = tuningSize;
    for (int t = 0; t < lookupTableSize; t++)
    {
        offset = t - rootIndex;
        frequency = calculateFrequencyFromRoot(offset);

        frequencyTable.set(t, frequency);
        double mts = roundN(10, frequencyToMTS(frequency));
        mtsTable.set(t, mts);

        dbgFreqTable[t] = frequency;
        dbgMtsTable[t] = mts;
    }
}

CentsDefinition FunctionalTuning::getDefinition() const
{
    return CentsDefinition{ centsTable, rootFrequency, name, description, getVirtualPeriod(), getVirtualSize() };
}


double FunctionalTuning::getPeriodCents() const
{
    return periodCents;
}

double FunctionalTuning::getPeriodSemitones() const
{
    return periodCents * 0.01;
}

double FunctionalTuning::getPeriodRatio() const
{
    return periodRatio;
}


juce::Array<double> FunctionalTuning::getIntervalCentsList() const
{
    // Don't include unison
    juce::Array<double> cents;
    for (int i = 1; i <= tuningSize; i++)
    {
        cents.set(i, centsMap->at(i));
    }

    return cents;
}

juce::Array<double> FunctionalTuning::getIntervalRatioList() const
{
    // Don't include unison
    juce::Array<double> ratios;
    for (int i = 1; i < tuningSize; i++)
    {
        ratios.set(i - 1, ratioTable[i]);
    }
    ratios.set(tuningSize - 1, periodRatio);

    return ratios;
}


double FunctionalTuning::calculateFrequencyFromRoot(int stepsFromRoot) const
{
    auto cents = centsMap->at(stepsFromRoot);
    auto ratio = centsToRatio(cents);
    return rootFrequency * ratio;
}

double FunctionalTuning::calculateMtsFromRoot(int stepsFromRoot) const
{
    auto freq = calculateFrequencyFromRoot(stepsFromRoot);
    return frequencyToMTS(freq);
}

double FunctionalTuning::calculateCentsFromRoot(int stepsFromRoot) const
{
    return centsFromRoot(stepsFromRoot);
}

double FunctionalTuning::calculateSemitonesFromRoot(int stepsFromRoot) const
{
    return calculateCentsFromRoot(stepsFromRoot) * 0.01;
}