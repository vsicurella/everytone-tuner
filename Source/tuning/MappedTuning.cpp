/*
  ==============================================================================

    MappedTuning.cpp
    Created: 24 Dec 2021 3:44:29pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "MappedTuning.h"

MappedTuningTable::MappedTuningTable(std::shared_ptr<TuningTable> tuningIn, std::shared_ptr<TuningTableMap> mappingIn)
    : tuning(tuningIn),
      mapping(mappingIn),
      TuningTableBase(tuningIn->getRootIndex(), tuningIn->getRootFrequency(), tuningIn->getName(), tuningIn->getDescription()) {}


MappedTuningTable::MappedTuningTable(const MappedTuningTable& mappedTuning)
    : tuning(mappedTuning.shareTuning()),
      mapping(mappedTuning.shareMapping()),
      TuningTableBase(mappedTuning.getTuning()->getRootIndex(), mappedTuning.getTuning()->getRootFrequency(), mappedTuning.getTuning()->getName(), mappedTuning.getTuning()->getDescription()) {}

MappedTuningTable::~MappedTuningTable()
{
    mapping = nullptr;
    tuning = nullptr;
}

int MappedTuningTable::getTuningSize() const
{
    return tuning->getTuningSize();
}

void MappedTuningTable::setRootFrequency(double frequency)
{
    // TODO change mapping?
    return tuning->setRootFrequency(frequency);
}

double MappedTuningTable::centsAt(int index) const
{
    return tuning->centsAt(index);
}

double MappedTuningTable::frequencyAt(int index) const
{
    return tuning->frequencyAt(index);
}

int MappedTuningTable::closestIndexToFrequency(double frequency) const
{
    return tuning->closestIndexToFrequency(frequency);
}

int MappedTuningTable::closestIndexToMts(double mts) const
{
    return tuning->closestIndexToMts(mts);
}

double MappedTuningTable::mtsAt(int index) const
{
    return tuning->mtsAt(index);
}

int MappedTuningTable::getTableSize() const
{
    return tuning->getTableSize();
}

double MappedTuningTable::getVirtualPeriod() const
{
    return tuning->getVirtualPeriod();
}

double MappedTuningTable::getVirtualSize() const
{
    return tuning->getVirtualSize();
}

double MappedTuningTable::getRootMts() const
{
    return tuning->getRootMts();
}

juce::Array<double> MappedTuningTable::getFrequencyTable() const
{
    return tuning->getFrequencyTable();
}

juce::Array<double> MappedTuningTable::getMtsTable() const
{
    return tuning->getMtsTable();
}

int MappedTuningTable::tuningIndexAt(int midiNote, int midiChannel) const
{
    auto mapped = mapping->getMappedNote(midiChannel, midiNote);
    return mapped.index;
}

double MappedTuningTable::centsAt(int midiNote, int midiChannel) const
{
    auto index = tuningIndexAt(midiNote, midiChannel);
    return tuning->centsAt(index);
}

double MappedTuningTable::frequencyAt(int midiNote, int midiChannel) const
{
    auto index = tuningIndexAt(midiNote, midiChannel);
    return tuning->frequencyAt(index);
}

double MappedTuningTable::mtsAt(int midiNote, int midiChannel) const
{
    auto index = tuningIndexAt(midiNote, midiChannel);
    return tuning->mtsAt(index);
}

