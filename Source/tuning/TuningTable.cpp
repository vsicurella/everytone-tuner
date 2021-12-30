/*
  ==============================================================================

    TuningTable.cpp
    Created: 26 Nov 2019 9:16:30pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "TuningTable.h"

TuningTable::TuningTable(CentsDefinition definition)
    : tuningSize(definition.intervalCents.size()),
      virtualPeriod(definition.virtualPeriod),
      virtualSize(definition.virtualSize),
      TuningBase(definition.rootFrequency, definition.name, definition.description)
{
	setupCentsMap(definition.intervalCents);
}

TuningTable::TuningTable(const TuningTable& tuning)
    : tuningSize(tuning.tuningSize),
      virtualPeriod(tuning.virtualPeriod),
      virtualSize(tuning.virtualSize),
      TuningBase(tuning.rootFrequency, tuning.name, tuning.description)
{
    setupCentsMap(tuning.getIntervalCentsList());
}

void TuningTable::setupCentsMap(const juce::Array<double>& cents)
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

    rebuildTables();
}

void TuningTable::setupRootAndTableSize()
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

void TuningTable::setRootFrequency(double frequency)
{
    // TODO check mts limits?
    rootFrequency = frequency;
    rebuildTables();
}

double TuningTable::centsAt(int index) const
{
	return centsMap->at(index);
}

double TuningTable::frequencyAt(int index) const
{
    auto tableIndex = mod(index, lookupTableSize);
    return frequencyTable[tableIndex];
}

double TuningTable::mtsAt(int index) const
{
    auto tableIndex = mod(index, lookupTableSize);
    return mtsTable[tableIndex];
}

int TuningTable::closestIndexToFrequency(double frequency) const
{
    return closestIndexToFrequency(frequency, true);
}

int TuningTable::closestIndexToFrequency(double frequency, bool useLookup) const
{
    double centsFromRoot = ratioToCents(frequency / rootFrequency);
    return closestIndexToCents(centsFromRoot);
}

int TuningTable::closestIndexToCents(double centsFromRoot) const
{
    if (tuningSize == 1)
    {
        auto stepsFromRoot = roundN(6, centsFromRoot / periodCents);
        return (int)round(rootIndex + stepsFromRoot);
    }

    // This assumes the scale pattern doesn't have intervals that jump beyond the period

    return centsMap->closestIndexTo(centsFromRoot) + rootIndex;
}

bool TuningTable::operator==(const TuningTable& tuning)
{
    return getDefinition() == tuning.getDefinition();
}

bool TuningTable::operator!=(const TuningTable& tuning)
{
    return !operator==(tuning);
}

CentsDefinition TuningTable::getDefinition() const
{
    return CentsDefinition{ centsTable, rootFrequency, name, description, virtualPeriod, virtualSize };
}

int TuningTable::getTuningTableSize() const
{
    return lookupTableSize;
}

double TuningTable::getVirtualPeriod() const
{
    return virtualPeriod;
}

double TuningTable::getVirtualSize() const
{
    return virtualSize;
}

double TuningTable::getPeriodCents() const
{
    return periodCents;
}

double TuningTable::getPeriodSemitones() const
{
    return periodCents * 0.01;
}

double TuningTable::getPeriodRatio() const
{
    return periodRatio;
}

double TuningTable::getRootMts() const
{
    return rootMts;
}

int TuningTable::getScaleDegree(int index) const
{
    return centsMap->mapIndexAt(index);
}

juce::Array<double> TuningTable::getIntervalCentsList() const
{
    // Don't include unison
    juce::Array<double> cents;
    for (int i = 1; i <= tuningSize; i++)
    {
        cents.set(i, centsMap->at(i));
    }

    return cents;
}

juce::Array<double> TuningTable::getIntervalRatioList() const
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

juce::Array<double> TuningTable::getFrequencyTable() const
{
    return frequencyTable;
}

juce::Array<double> TuningTable::getMtsTable() const
{
    return mtsTable;
}

double TuningTable::calculateFrequencyFromRoot(int stepsFromRoot) const
{
    auto cents = centsMap->at(stepsFromRoot);
    auto ratio = centsToRatio(cents);
    return rootFrequency * ratio;
}

double TuningTable::calculateMtsFromRoot(int stepsFromRoot) const
{
    auto freq = calculateFrequencyFromRoot(stepsFromRoot);
    return frequencyToMTS(freq);
}

double TuningTable::calculateCentsFromRoot(int stepsFromRoot) const
{
    return centsFromRoot(stepsFromRoot);
}

double TuningTable::calculateSemitonesFromRoot(int stepsFromRoot) const
{
    return calculateCentsFromRoot(stepsFromRoot) * 0.01;
}

//
//juce::Array<MTSTriplet> TuningTable::getMTSDataTable() const
//{
//    juce::Array<MTSTriplet> table;
//    for (int i = 0; i < TUNING_TABLE_SIZE; i++)
//        table.set(i, mtsNoteToTriplet(mtsTable[i]));
//    return table;
//}
//
//template <typename U>
//juce::String TuningTable::tableToString(const juce::Array<U>& table, int startMidiChannel, int endMidiChannel) const
//{
//    juce::String tableStr = "";
//    for (int ch = startMidiChannel - 1; ch < endMidiChannel - 1; modulo(ch + 1, 16))
//    {
//        int offset = midiIndex(0, ch);
//        tableStr << "Channel " << juce::String(ch + 1) << ":" << juce::newLine;
//
//        for (int i = 0; i < 128; i++)
//            tableStr << juce::String(table[offset + i]) << juce::newLine;
//    }
//    return tableStr;
//}

//juce::String TuningTable::mtsTableToString(int startMidiChannel, int endMidiChannel) const
//{
//    return tableToString<double>(mtsTable, startMidiChannel, endMidiChannel);
//}
