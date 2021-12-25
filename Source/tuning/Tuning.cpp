/*
  ==============================================================================

    Tuning.cpp
    Created: 26 Nov 2019 9:16:30pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "Tuning.h"

Tuning::Tuning(CentsDefinition definition)
    : tuningSize(definition.intervalCents.size()),
      rootFrequency(definition.rootFrequency),
      TuningBase(definition.name, definition.description)
{
	setupCentsMap(definition.intervalCents);
}

Tuning::Tuning(const Tuning& tuning)
    : tuningSize(tuning.tuningSize),
      rootFrequency(tuning.rootFrequency),
      periodCents(tuning.periodCents),
      TuningBase(tuning.name, tuning.description)
{
    setupCentsMap(tuning.getIntervalCentsTable());
}

void Tuning::setupCentsMap(const juce::Array<double>& cents)
{
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
        0,         /* pattern rootIndex */
        rootIndex, /* tuning index rootIndex */
    };

    centsMap.reset(new Map<double>(definition));

    rebuildTables();
}

void Tuning::setupRootAndTableSize()
{
    double lowestRatio = MTS_LOWEST_FREQ / rootFrequency;
    double lowestCents = ratioToCents(lowestRatio);
    int lowestFromRoot = centsMap->closestIndexTo(lowestCents);

    double highestRatio = MTS_HIGHEST_FREQ / rootFrequency;
    double highestCents = ratioToCents(highestRatio);
    int highestFromRoot = centsMap->closestIndexTo(highestCents);

    rootIndex = -lowestFromRoot;
    lookupTableSize = lowestFromRoot - highestFromRoot;

    //juce::Logger::writeToLog("Best mapping, rootIndex: " + juce::String(lowestToNewRoot) + ", size: " + juce::String(tableSize));
}

void Tuning::rebuildTables()
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

void Tuning::setRootFrequency(double frequency)
{
    // TODO check mts limits?
    rootFrequency = frequency;
    rebuildTables();
}

double Tuning::centsAt(int index) const
{
	return centsMap->at(index);
}

double Tuning::frequencyAt(int index) const
{
    auto index = mod(index, lookupTableSize);
    return frequencyTable[index];
}

double Tuning::mtsAt(int index) const
{
    auto index = mod(index, lookupTableSize);
    return mtsTable[index];
}

int Tuning::closestIndexToFrequency(double frequency) const
{
    return closestIndexToFrequency(frequency, true);
}

int Tuning::closestIndexToFrequency(double frequency, bool useLookup) const
{
    double centsFromRoot = ratioToCents(frequency / rootFrequency);
    return closestIndexToCents(centsFromRoot);
}

int Tuning::closestIndexToCents(double centsFromRoot) const
{
    if (tuningSize == 1)
    {
        return rootIndex + (int)round(centsFromRoot / periodCents);
    }

    // This assumes the scale pattern doesn't have intervals that jump beyond the period

    return centsMap->closestIndexTo(centsFromRoot);
}

bool Tuning::operator==(const Tuning& tuning)
{
    return getDefinition() == tuning.getDefinition();
}

bool Tuning::operator!=(const Tuning& tuning)
{
    return !operator==(tuning);
}

CentsDefinition Tuning::getDefinition() const
{
    return CentsDefinition{ centsTable, rootFrequency, name, description, virtualPeriod, virtualSize };
}

double Tuning::getVirtualPeriod() const
{
    return virtualPeriod;
}

double Tuning::getVirtualSize() const
{
    return virtualSize;
}

double Tuning::getPeriodCents() const
{
    return periodCents;
}

double Tuning::getPeriodSemitones() const
{
    return periodCents * 0.01;
}

double Tuning::getPeriodRatio() const
{
    return periodRatio;
}

double Tuning::getRootMts() const
{
    return rootMts;
}

int Tuning::getScaleDegree(int index) const
{
    return centsMap->mapIndexAt(index);
}

juce::Array<double> Tuning::getIntervalCentsTable() const
{
    // Don't include unison
    juce::Array<double> cents;
    for (int i = 1; i <= tuningSize; i++)
    {
        cents.set(i, centsMap->at(i));
    }

    return cents;
}

juce::Array<double> Tuning::getIntervalRatioTable() const
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

juce::Array<double> Tuning::getFrequencyTable() const
{
    return frequencyTable;
}

juce::Array<double> Tuning::getMtsTable() const
{
    return mtsTable;
}

double Tuning::calculateFrequencyFromRoot(int stepsFromRoot) const
{
    auto cents = centsMap->at(stepsFromRoot);
    auto ratio = centsToRatio(cents);
    return rootFrequency * ratio;
}

double Tuning::calculateMtsFromRoot(int stepsFromRoot) const
{
    auto freq = calculateFrequencyFromRoot(stepsFromRoot);
    return frequencyToMTS(freq);
}

double Tuning::calculateCentsFromRoot(int stepsFromRoot) const
{
    return centsFromRoot(stepsFromRoot);
}

double Tuning::calculateSemitonesFromRoot(int stepsFromRoot) const
{
    return calculateCentsFromRoot(stepsFromRoot) * 0.01;
}

//
//juce::Array<MTSTriplet> Tuning::getMTSDataTable() const
//{
//    juce::Array<MTSTriplet> table;
//    for (int i = 0; i < TUNING_TABLE_SIZE; i++)
//        table.set(i, mtsNoteToTriplet(mtsTable[i]));
//    return table;
//}
//
//template <typename U>
//juce::String Tuning::tableToString(const juce::Array<U>& table, int startMidiChannel, int endMidiChannel) const
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

//juce::String Tuning::mtsTableToString(int startMidiChannel, int endMidiChannel) const
//{
//    return tableToString<double>(mtsTable, startMidiChannel, endMidiChannel);
//}
