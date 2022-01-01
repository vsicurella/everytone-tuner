/*
  ==============================================================================

    FunctionalTuning.cpp
    Created: 29 Dec 2021 10:27:23pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "FunctionalTuning.h"

FunctionalTuning::FunctionalTuning(CentsDefinition definition, bool buildTables)
    : TuningTable(setupEmptyTableDefinition(definition)) /* initialize without table size */
{
    setupCentsMap(definition.intervalCents);
    TuningTable::setRootFrequency(definition.rootFrequency);

    int tableSize;
    definition.calculateMtsRootAndTableSize(rootIndex, tableSize);
    setTableSize(tableSize);

    if (buildTables)
    {
        cacheTables();
    }
}

FunctionalTuning::FunctionalTuning(const FunctionalTuning& tuning)
    : TuningTable(static_cast<const TuningTable&>(tuning)),
      centsMap(tuning.centsMap),
      tablesAreBuilt(tuning.tablesAreBuilt),
      tuningSize(tuning.tuningSize),
      periodCents(tuning.periodCents),
      periodRatio(tuning.periodRatio)
{
    setTableSize(tuning.getTableSize());
}

void FunctionalTuning::setupCentsMap(const juce::Array<double>& cents)
{
    tuningSize  = cents.size();
    periodCents = cents.getLast();
    periodRatio = centsToRatio(periodCents);

    std::vector<double> tuningShifted;
    tuningShifted.push_back(0.0);

    for (int i = 0; i < cents.size() - 1; i++)
        tuningShifted.push_back(cents[i]);
    
    Map<double>::Definition definition =
    {
        tuningSize,
        tuningShifted,
        periodCents,
        0, /* pattern rootIndex */
        0, /* tuning index rootIndex */
    };

    centsMap = Map<double>(definition);
}

//int FunctionalTuning::setupRootIndexAndGetTableSize()
//{
//    int lookupTableSize;
//    calculateRootAndTableSizeFromMap(centsMap, rootFrequency, rootIndex, lookupTableSize);
//    return lookupTableSize;
//}

TuningTable::Definition FunctionalTuning::setupEmptyTableDefinition(const CentsDefinition& definition)
{
    juce::Array<double> emptyTable;
    TuningTable::Definition tableDefinition =
    {
        emptyTable,
        0,
        definition.name,
        definition.description,
        definition.getPeriodString(),
        definition.virtualPeriod,
        definition.virtualSize
    };

    return tableDefinition;
}

bool FunctionalTuning::operator==(const FunctionalTuning& tuning)
{
    // TODO stricter and looser versions
    return centsMap == tuning.centsMap
        && rootIndex == tuning.rootIndex
        && rootFrequency == tuning.rootFrequency;
}

bool FunctionalTuning::operator!=(const FunctionalTuning& tuning)
{
    return !operator==(tuning);
}

void FunctionalTuning::cacheTables()
{
    auto table = buildFrequencyTable();
    setTableWithFrequencies(table);
    tablesAreBuilt = true;
}

CentsDefinition FunctionalTuning::getDefinition() const
{
    return CentsDefinition { getIntervalCentsList(), rootFrequency, name, description, getVirtualPeriod(), getVirtualSize() };
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

double FunctionalTuning::calculateFrequencyFromRoot(int stepsFromRoot) const
{
    auto cents = centsMap.at(stepsFromRoot);
    auto ratio = centsToRatio(cents);
    return rootFrequency * ratio;
}

double FunctionalTuning::calculateMtsFromRoot(int stepsFromRoot) const
{
    auto freq = calculateFrequencyFromRoot(stepsFromRoot);
    return roundN(8, frequencyToMTS(freq));
}

double FunctionalTuning::calculateCentsFromRoot(int stepsFromRoot) const
{
    return centsFromRoot(stepsFromRoot);
}

double FunctionalTuning::calculateSemitonesFromRoot(int stepsFromRoot) const
{
    return calculateCentsFromRoot(stepsFromRoot) * 0.01;
}

juce::Array<double> FunctionalTuning::getIntervalCentsList() const
{
    // Don't include unison
    juce::Array<double> cents;
    for (int i = 1; i <= tuningSize; i++)
    {
        cents.add(centsMap.at(i));
    }

    return cents;
}

juce::Array<double> FunctionalTuning::getIntervalRatioList() const
{
    // Don't include unison
    juce::Array<double> ratios;
    for (int i = 1; i < tuningSize; i++)
    {
        auto cents = centsMap.at(i);
        ratios.add(centsToRatio(cents));
    }
    ratios.add(periodRatio);

    return ratios;
}

void FunctionalTuning::setRootFrequency(double frequency)
{
    rootFrequency = frequency;

    int tableSize;
    auto cents = getIntervalCentsList();
    CentsDefinition::calculateMtsRootAndTableSize(cents, rootFrequency, rootIndex, tableSize);
    
    if (tablesAreBuilt)
    {
        auto frequencyTable = buildFrequencyTable(tableSize);
        return setTableWithFrequencies(frequencyTable);
    }

    setTableSize(tableSize);
}

int FunctionalTuning::getTableSize() const
{
    return getTableSize(false);
}

int FunctionalTuning::getTableSize(bool calculate) const
{
    if (calculate)
    {
        int root, size;
        auto cents = getIntervalCentsList();
        CentsDefinition::calculateMtsRootAndTableSize(cents, rootFrequency, root, size);
        return size;
    }
    
    return TuningTable::getTableSize();
}

juce::Array<double> FunctionalTuning::getFrequencyTable() const
{
    if (tablesAreBuilt)
        return TuningTable::getFrequencyTable();

    return buildFrequencyTable();
}

juce::Array<double> FunctionalTuning::getMtsTable() const
{
    if (tablesAreBuilt)
        return TuningTable::getMtsTable();

    auto frequencies = buildFrequencyTable();
    return frequencyToMtsTable(frequencies);
}

double FunctionalTuning::centsAt(int index) const
{
    if (tablesAreBuilt)
        return TuningTable::centsAt(index);

    return calculateCentsFromRoot(index - rootIndex);
}

double FunctionalTuning::frequencyAt(int index) const
{
    if (tablesAreBuilt)
        return TuningTable::frequencyAt(index);

    return calculateFrequencyFromRoot(index - rootIndex);
}

double FunctionalTuning::mtsAt(int index) const
{
    if (tablesAreBuilt)
        return TuningTable::mtsAt(index);

    return calculateMtsFromRoot(index - rootIndex);
}

int FunctionalTuning::closestIndexToFrequency(double frequency) const
{
    if (tablesAreBuilt)
        return TuningTable::closestIndexToFrequency(frequency);

    auto cents = ratioToCents(frequency / rootFrequency);
    return closestIndexToCents(cents);
}

int FunctionalTuning::closestIndexToCents(double centsFromRoot) const
{
    if (tablesAreBuilt)
        return TuningTable::closestIndexToCents(centsFromRoot);

    return centsMap.closestIndexTo(centsFromRoot) + rootIndex;
}

juce::Array<double> FunctionalTuning::buildFrequencyTable(int tableSize) const
{
    juce::Array<double> frequencies;

    if (tableSize == 0)
        tableSize = getTableSize();

    if (tableSize == 0)
        return frequencies;

    for (int i = 0; i < tableSize; i++)
    {
        auto index = i - rootIndex;
        auto f = calculateFrequencyFromRoot(index);
        frequencies.add(f);
    }

    return frequencies;
}
//
//void FunctionalTuning::calculateRootAndTableSizeFromMap(const Map<double>& centsMap, double rootFrequency, int& rootIndex, int& tableSize)
//{
//    double lowestRatio = MTS_LOWEST_FREQ / rootFrequency;
//    double lowestCents = ratioToCents(lowestRatio);
//    int lowestFromRoot = centsMap.closestIndexTo(lowestCents);
//
//    double highestRatio = MTS_HIGHEST_FREQ / rootFrequency;
//    double highestCents = ratioToCents(highestRatio);
//    int highestFromRoot = centsMap.closestIndexTo(highestCents);
//
//    rootIndex = -lowestFromRoot;
//    tableSize = highestFromRoot - lowestFromRoot + 1;
//}