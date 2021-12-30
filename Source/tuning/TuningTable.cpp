/*
  ==============================================================================

    TuningTable.cpp
    Created: 26 Nov 2019 9:16:30pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "TuningTable.h"

TuningTable::TuningTable(TuningTable::Definition definition)
    : frequencyTable(definition.frequencies),
      periodString(definition.periodString),
      virtualPeriod(definition.virtualPeriod),
      virtualSize(definition.virtualSize),
      TuningBase(definition.frequencies[definition.rootIndex], definition.name, definition.description)
      
{
    refreshTableMetadata();
}

TuningTable::TuningTable(const TuningTable& tuning)
    : frequencyTable(tuning.frequencyTable),
      periodString(tuning.periodString),
      virtualPeriod(tuning.virtualPeriod),
      virtualSize(tuning.virtualSize),
      TuningBase(tuning.rootFrequency, tuning.name, tuning.description),
      mtsTable(tuning.mtsTable),
      rootMts(tuning.rootMts)
{
}

void TuningTable::refreshTableMetadata()
{
    // Rebuild MTS table
    mtsTable = frequencyToMtsTable(frequencyTable);
    rootMts = frequencyToMTS(rootFrequency);
}

void TuningTable::setVirtualPeriod(double period, juce::String periodStr = "")
{
    virtualPeriod = period;
    periodString = periodStr;
}

void TuningTable::setVirtualSize(double size)
{
    virtualSize = size;
}

void TuningTable::setTableWithFrequencies(juce::Array<double> frequencies, int newRootIndex)
{
    if (newRootIndex < 0)
        newRootIndex = rootIndex;

    
}

void TuningTable::setTableWithMts(juce::Array<double> mts, int newRootIndex)
{
    if (newRootIndex < 0)
        newRootIndex = rootIndex;


}

void TuningTable::setRootIndex(int newRootIndex)
{
    rootIndex = newRootIndex;
    rootFrequency = frequencyTable[rootIndex]; // probably should do some checking
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
    auto tableIndex = mod(index, getTableSize());
    return frequencyTable[tableIndex];
}

double TuningTable::mtsAt(int index) const
{
    auto tableIndex = mod(index, getTableSize());
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

int TuningTable::getTableSize() const
{
    return frequencyTable.size();
}

double TuningTable::getVirtualPeriod() const
{
    return virtualPeriod;
}

double TuningTable::getVirtualSize() const
{
    return virtualSize;
}

double TuningTable::getRootMts() const
{
    return rootMts;
}

juce::Array<double> TuningTable::getIntervalCentsList() const
{
    juce::Array<double> cents;
    for (int i = 1; i <= tuningSize; i++)
    {
        cents.set(i, centsMap->at(i));
    }

    return cents;
}

juce::Array<double> TuningTable::getIntervalRatioList() const
{
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

juce::Array<double> TuningTable::frequencyToMtsTable(juce::Array<double> frequenciesIn)
{
    auto mtsTable = juce::Array<double>(frequenciesIn.size());
    for (int n = 0; n < frequenciesIn.size(); n++)
    {
        auto freq = frequenciesIn[n];
        double mts = roundN(10, frequencyToMTS(freq));
        mtsTable.set(n, mts);
    }
    return mtsTable;
}

juce::Array<double> TuningTable::mtsToFrequencyTable(juce::Array<double> mtsIn)
{
    auto frequencyTable = juce::Array<double>(mtsIn.size());
    for (int n = 0; n < mtsIn.size(); n++)
    {
        auto mts = mtsIn[n];
        double frequency = roundN(10, mtsToFrequency(mts));
        frequencyTable.set(n, mts);
    }
    return frequencyTable;
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
