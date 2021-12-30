/*
  ==============================================================================

    MappedTuning.cpp
    Created: 24 Dec 2021 3:44:29pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "MappedTuning.h"

MappedTuning::MappedTuning(std::shared_ptr<TuningTable> tuningIn, std::shared_ptr<TuningTableMap> mappingIn)
    : tuning(tuningIn),
      mapping(mappingIn),
      TuningBase(tuningIn->getRootIndex(), tuningIn->getRootFrequency(), tuningIn->getName(), tuningIn->getDescription()) {}


MappedTuning::MappedTuning(const MappedTuning& mappedTuning)
    : tuning(mappedTuning.shareTuning()),
      mapping(mappedTuning.shareMapping()),
      TuningBase(mappedTuning.getTuning()->getRootIndex(), mappedTuning.getTuning()->getRootFrequency(), mappedTuning.getTuning()->getName(), mappedTuning.getTuning()->getDescription()) {}

MappedTuning::~MappedTuning()
{
    mapping = nullptr;
    tuning = nullptr;
}

int MappedTuning::getTuningSize() const
{
    return tuning->getTuningSize();
}

void MappedTuning::setRootFrequency(double frequency)
{
    // TODO change mapping?
    return tuning->setRootFrequency(frequency);
}

juce::Array<double> MappedTuning::getIntervalCentsList() const
{
    return tuning->getIntervalCentsList();
}

double MappedTuning::centsAt(int index) const
{
    return tuning->centsAt(index);
}

double MappedTuning::frequencyAt(int index) const
{
    return tuning->frequencyAt(index);
}

int MappedTuning::closestIndexToFrequency(double frequency) const
{
    return tuning->closestIndexToFrequency(frequency);
}

int MappedTuning::closestIndexToMts(double mts) const
{
    return tuning->closestIndexToMts(mts);
}

double MappedTuning::mtsAt(int index) const
{
    return tuning->mtsAt(index);
}

int MappedTuning::tuningIndexAt(int midiNote, int midiChannel) const
{
    auto mapped = mapping->getMappedNote(midiChannel, midiNote);
    return mapped.index;
}

double MappedTuning::centsAt(int midiNote, int midiChannel) const
{
    auto index = tuningIndexAt(midiNote, midiChannel);
    return tuning->centsAt(index);
}

double MappedTuning::frequencyAt(int midiNote, int midiChannel) const
{
    auto index = tuningIndexAt(midiNote, midiChannel);
    return tuning->frequencyAt(index);
}

double MappedTuning::mtsAt(int midiNote, int midiChannel) const
{
    auto index = tuningIndexAt(midiNote, midiChannel);
    return tuning->mtsAt(index);
}
