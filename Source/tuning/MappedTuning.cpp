/*
  ==============================================================================

    MappedTuning.cpp
    Created: 24 Dec 2021 3:44:29pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "MappedTuning.h"

int MappedTuningTable::getTranspositionForReference(TuningTableMap::Root root, FrequencyReference reference, int tuningTableSize)
{
    if (reference.isInvalid())
        return 0;

    auto transposition = midiIndex(root.midiChannel, root.midiNote) - midiIndex(reference.midiChannel, reference.midiNote);
    return mod(transposition, tuningTableSize);
}

MappedTuningTable::MappedTuningTable(std::shared_ptr<TuningTable> tuningIn, std::shared_ptr<TuningTableMap> mappingIn, FrequencyReference referenceIn)
    : TuningTableBase(tuningIn->getRootIndex(), tuningIn->getRootFrequency(), tuningIn->getName(), tuningIn->getDescription()) ,
      tuning(tuningIn),
      mapping(mappingIn),
      reference(referenceIn)
{
    alignMappingWithReference();
}

MappedTuningTable::MappedTuningTable(const MappedTuningTable& mappedTuning)
    : TuningTableBase(mappedTuning.getTuning()->getRootIndex(), mappedTuning.getTuning()->getRootFrequency(), mappedTuning.getTuning()->getName(), mappedTuning.getTuning()->getDescription()),
      tuning(mappedTuning.shareTuning()),
      mapping(mappedTuning.shareMapping()),
      reference(mappedTuning.reference)
      
{
    alignMappingWithReference();
}

MappedTuningTable::~MappedTuningTable()
{
    mapping = nullptr;
    tuning = nullptr;
}

void MappedTuningTable::alignMappingWithReference()
{
    auto transposition = getTranspositionForReference(mapping->getRoot(), reference, tuning->getTableSize());
    if (transposition != mapping->getTransposition())
        mapping = mapping->withTransposition(transposition);
}

//std::shared_ptr<MappedTuningTable> MappedTuningTable::withReference(FrequencyReference reference) const
//{
//    if (reference == tuning->getReference())
//        return nullptr;
//
//    auto newTransposition = getTranspositionForReference();
//    if (newTransposition == mapping->getTransposition())
//        return nullptr;
//
//    auto newMapping = mapping->withTransposition(newTransposition);
//    return std::make_shared<MappedTuningTable>(tuning, newMapping);
//}

int MappedTuningTable::getTuningSize() const
{
    auto functional = dynamic_cast<FunctionalTuning*>(tuning.get());
    if (functional != nullptr)
        return functional->getTuningSize();

    auto virtualSize = tuning->getVirtualSize();
    if (virtualSize != 0)
        return round(virtualSize); // ??

    return tuning.get()->getTableSize();
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

//std::shared_ptr<MappedTuningTable> MappedTuningTable::setRoot(MappedTuningTable::Root root, bool returnCopies)
//{
//    auto reference = root.tuningReference;
//
//    auto newTuning = tuning;
//    if (root.frequency != newTuning->getRootFrequency())
//    {
//        newTuning = setRootFrequency(root.frequency, true);
//    }
//
//    auto newMapping = mapping;
//    if (root.mapping != mapping->getRoot())
//    {
//
//    }
//}

int MappedTuningTable::getTableSize() const
{
    return tuning->getTableSize();
}

juce::String MappedTuningTable::getPeriodString() const
{
    return tuning->getPeriodString();
}

juce::String MappedTuningTable::getSizeString() const
{
    return tuning->getSizeString();
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

double MappedTuningTable::mtsAt(int index) const
{
    return tuning->mtsAt(index);
}

juce::Array<double> MappedTuningTable::getFrequencyTable() const
{
    return tuning->getFrequencyTable();
}

juce::Array<double> MappedTuningTable::getMtsTable() const
{
    return tuning->getMtsTable();
}

void MappedTuningTable::setRootFrequency(double frequency)
{
    setRootFrequency(frequency, false);
}

std::shared_ptr<TuningTable> MappedTuningTable::setRootFrequency(double frequency, bool returnNewTuning)
{
    if (returnNewTuning)
    {
        auto functional = dynamic_cast<FunctionalTuning*>(tuning.get());
        if (functional != nullptr)
        {
            auto definition = functional->getDefinition();
            definition.rootFrequency = frequency;
            auto newTuning = std::make_shared<FunctionalTuning>(definition);
            return newTuning;
        }

        auto copy = std::make_shared<TuningTable>(*tuning);
        copy->setRootFrequency(frequency);
        return copy;
    }

    tuning->setRootFrequency(frequency);
    return tuning;
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

std::shared_ptr<TuningTableMap> MappedTuningTable::LinearMappingFromTuning(const TuningTable* tuning, TuningTableMap::Root root)
{
    auto definition = TuningTableMap::LinearMappingDefinition(root.midiChannel, root.midiNote, tuning->getRootIndex(), tuning->getTableSize());
    return std::make_shared<TuningTableMap>(definition);
}

std::shared_ptr<TuningTableMap> MappedTuningTable::PeriodicMappingFromTuning(const TuningTable* tuning, TuningTableMap::Root root)
{
    auto definition = MultichannelMap::PeriodicMappingDefinition((int)tuning->getVirtualSize(), root.midiChannel, root.midiNote, tuning->getRootIndex(), tuning->getTableSize());
    return std::make_shared<TuningTableMap>(definition);
}
