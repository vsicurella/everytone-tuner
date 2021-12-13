/*
  ==============================================================================

    MappedTuningController.cpp
    Created: 11 Dec 2021 9:43:20pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "MappedTuningController.h"

MappedTuningController::MappedTuningController()
{
    tunings.add(std::make_unique<Tuning>(Tuning::StandardTuning()));
    currentTuningSource = tunings.getLast();
    currentTuningTarget = tunings.getLast();
}

MappedTuningController::~MappedTuningController()
{

}

void MappedTuningController::setSourceTuning(const Tuning* tuning, bool updateTuner)
{
    tunings.add(std::make_unique<Tuning>(tuning));
    currentTuningSource = tunings.getLast();
    if (updateTuner)
        updateCurrentTuner();
}


void MappedTuningController::setSourceTuning(const Tuning* tuning)
{
    setSourceTuning(tuning, true);
}

void MappedTuningController::setTargetTuning(const Tuning* tuning, bool updateTuner)
{
    tunings.add(std::make_unique<Tuning>(tuning));
    currentTuningTarget = tunings.getLast();
    if (updateTuner)
        updateCurrentTuner();
}

void MappedTuningController::setTargetTuning(const Tuning* tuning)
{
    setTargetTuning(tuning, true);
}

void MappedTuningController::setNoteMapping(const Keytographer::TuningTableMap* mapping, bool updateTuner)
{
    mappings.add(std::make_unique<Keytographer::TuningTableMap>(mapping));
    currentMapping = mappings.getLast();
    if (updateTuner)
        updateCurrentTuner();
}

void MappedTuningController::setNoteMapping(const Keytographer::TuningTableMap* mapping)
{
    setNoteMapping(mapping);
}

void MappedTuningController::setTunings(const Tuning* sourceTuning, const Tuning* targetTuning, const Keytographer::TuningTableMap* mapping)
{
    if (sourceTuning != nullptr)
    {
        setSourceTuning(sourceTuning, false);
    }

    if (targetTuning != nullptr)
    {
        setTargetTuning(targetTuning, false);
    }

    if (mapping != nullptr)
    {
        setNoteMapping(mapping, false);
    }

    updateCurrentTuner();
}

void MappedTuningController::updateCurrentTuner()
{
    tuners.add(std::make_shared<MidiNoteTuner>(currentTuningSource, currentTuningTarget, currentMapping));
    currentTuner = tuners.getLast();
}

void MappedTuningController::setMappingType(Multimapper::MappingType type)
{
    mappingType = type;
    if (mappingMode == Multimapper::MappingMode::Auto)
        updateAutoMapping(true);
}

void MappedTuningController::updateAutoMapping(bool updateTuner)
{
    auto mapping = std::make_unique<Keytographer::TuningTableMap>(newTuningMap(currentTuningTarget.get()));
    setNoteMapping(mapping.get(), updateTuner);
}

Keytographer::TuningTableMap* MappedTuningController::newTuningMap(const Tuning* tuning)
{
    return newTuningMap(tuning, mappingType);
}

Keytographer::TuningTableMap* MappedTuningController::NewLinearMappingFromTuning(const Tuning* tuning)
{
    auto mapFunction = Keytographer::Map<int>::FunctionDefinition
    {
        2048,
        tuning->getRootIndex(),
        [&](int x) { return modulo(x - tuning->getRootIndex(), 2048); }
    };
    auto linearMap = Keytographer::Map<int>(mapFunction);
    auto definition = Keytographer::TuningTableMap::Definition
    {
        tuning->getRootMidiNote(), tuning->getRootIndex(), &linearMap
    };
   
    auto map = new Keytographer::TuningTableMap(definition);
    return map;
}

Keytographer::TuningTableMap* MappedTuningController::NewPeriodicMappingFromTuning(const Tuning* tuning)
{
    auto periodicMap = Keytographer::MultichannelMap::CreatePeriodicMapping(tuning->getTuningSize(), tuning->getRootMidiNote(), tuning->getRootMidiChannel());
    auto map = new Keytographer::TuningTableMap(periodicMap);
    return map;
}

Keytographer::TuningTableMap* MappedTuningController::newTuningMap(const Tuning* tuning, Multimapper::MappingType mappingType)
{
    switch (mappingType)
    {
    case Multimapper::MappingType::Linear:
    {
        return NewLinearMappingFromTuning(tuning);
    }

    case Multimapper::MappingType::Periodic:
    {
        return NewPeriodicMappingFromTuning(tuning);
    }

    default:
        jassertfalse;
    }

    return nullptr;
}