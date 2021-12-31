/*
  ==============================================================================

    FunctionalTuning.cpp
    Created: 29 Dec 2021 10:27:23pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "FunctionalTuning.h"

FunctionalTuning::FunctionalTuning(CentsDefinition definition)
    : TuningTable(setupEmptyTableDefinition(definition))
{
}

FunctionalTuning::FunctionalTuning(CentsDefinition definition, bool buildTables)
    : TuningTable((buildTables) ? setupFrequencyTableDefinition(definition) : setupEmptyTableDefinition(definition))
{
}

FunctionalTuning::FunctionalTuning(const FunctionalTuning& tuning)
    : TuningTable((tuning.tablesAreBuilt) ? setupFrequencyTableDefinition(tuning.getDefinition()) : setupEmptyTableDefinition(tuning.getDefinition()))
{

}

void FunctionalTuning::setupCentsMap(const juce::Array<double>& cents)
{
    centsTable = cents;
    tuningSize = centsTable.size();

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
        0, /* pattern rootIndex */
        0, /* tuning index rootIndex */
    };

    centsMap.reset(new Map<double>(definition));
}

int FunctionalTuning::setupRootIndexAndGetTableSize()
{
    int lookupTableSize;
    calculateRootAndTableSizeFromMap(centsMap.get(), rootFrequency, rootIndex, lookupTableSize);
    return lookupTableSize;
}

TuningTable::Definition FunctionalTuning::setupEmptyTableDefinition(const CentsDefinition& definition)
{
    setupCentsMap(definition.intervalCents);

    auto tableSize = setupRootIndexAndGetTableSize();

    juce::Array<double> emptyTable;
    emptyTable.resize(tableSize);
    emptyTable.fill(0);
    tablesAreBuilt = false;

    TuningTable::Definition tableDefinition =
    {
        emptyTable,
        definition.rootFrequency,
        definition.name,
        definition.description,
        definition.getPeriodString(),
        definition.virtualPeriod,
        definition.virtualSize
    };

    return tableDefinition;
}

TuningTable::Definition FunctionalTuning::setupFrequencyTableDefinition(const CentsDefinition& definition)
{
    setupCentsMap(definition.intervalCents);

    auto tableSize = setupRootIndexAndGetTableSize();

    auto table = buildFrequencyTable(tableSize);
    tablesAreBuilt = true;
    
    TuningTable::Definition tableDefinition =
    {
        table,
        definition.rootFrequency,
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
    return centsTable == tuning.centsTable,
        rootIndex == tuning.rootIndex,
        rootFrequency == tuning.rootFrequency;
}

bool FunctionalTuning::operator!=(const FunctionalTuning& tuning)
{
    return !operator==(tuning);
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

juce::Array<double> FunctionalTuning::getIntervalCentsList() const
{
    // Don't include unison
    juce::Array<double> cents;
    for (int i = 1; i <= tuningSize; i++)
    {
        cents.add(centsMap->at(i));
    }

    return cents;
}

juce::Array<double> FunctionalTuning::getIntervalRatioList() const
{
    // Don't include unison
    juce::Array<double> ratios;
    for (int i = 1; i < tuningSize; i++)
    {
        auto ratio = centsToRatio(centsTable[i]);
        ratios.add(ratio);
    }
    ratios.add(periodRatio);

    return ratios;
}

void FunctionalTuning::setRootFrequency(double frequency)
{
    rootFrequency = frequency;
    int tableSize = setupRootIndexAndGetTableSize();
    
    if (tablesAreBuilt)
        return setTableWithFrequencies(buildFrequencyTable(tableSize));

    juce::Array<double> emptyTable;
    emptyTable.resize(tableSize);
    setTableWithFrequencies(emptyTable);
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
        calculateRootAndTableSizeFromMap(centsMap.get(), rootFrequency, root, size);
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
        return TuningTable::getFrequencyTable();

    auto frequencies = buildFrequencyTable();
    return mtsToFrequencyTable(frequencies);
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

    return centsMap->closestIndexTo(centsFromRoot);
}

juce::Array<double> FunctionalTuning::buildFrequencyTable(int tableSize) const
{
    auto frequencies = juce::Array<double>();

    if (tableSize == 0)
        tableSize = getTableSize();

    if (tableSize == 0)
        return frequencies;

    for (int i = 0; i < getTableSize(); i++)
    {
        auto index = i - rootIndex;
        auto f = calculateFrequencyFromRoot(index);
        frequencies.add(f);
    }

    return frequencies;
}

void FunctionalTuning::calculateRootAndTableSizeFromMap(Map<double>* centsMap, double rootFrequency, int& rootIndex, int& tableSize)
{
    double lowestRatio = MTS_LOWEST_FREQ / rootFrequency;
    double lowestCents = ratioToCents(lowestRatio);
    int lowestFromRoot = centsMap->closestIndexTo(lowestCents);

    double highestRatio = MTS_HIGHEST_FREQ / rootFrequency;
    double highestCents = ratioToCents(highestRatio);
    int highestFromRoot = centsMap->closestIndexTo(highestCents);

    rootIndex = -lowestFromRoot;
    tableSize = highestFromRoot - lowestFromRoot + 1;
}