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
    updateAutoMapping(true);
}

MappedTuningController::~MappedTuningController()
{

}

void MappedTuningController::setSourceTuning(const Tuning* tuning, bool updateTuner)
{
    tunings.add(std::make_unique<Tuning>(*tuning));
    currentTuningSource = tunings.getLast();
    juce::Logger::writeToLog("Loaded new source tuning: " + tuning->getDescription());
    if (updateTuner)
        updateCurrentTuner();
}


void MappedTuningController::setSourceTuning(const Tuning* tuning)
{
    setSourceTuning(tuning, true);
}

void MappedTuningController::setTargetTuning(const Tuning* tuning, bool updateTuner)
{
    auto lastTuning = currentTuningTarget;
    tunings.add(std::make_unique<Tuning>(*tuning));
    currentTuningTarget = tunings.getLast(); 

    juce::Logger::writeToLog("Loaded new target tuning: " + tuning->getDescription());

    if (mappingMode == Multimapper::MappingMode::Auto && *lastTuning != *currentTuningTarget)
        updateAutoMapping(false);

    if (updateTuner)
        updateCurrentTuner();
}

void MappedTuningController::setTargetTuning(const Tuning* tuning)
{
    setTargetTuning(tuning, true);
}

void MappedTuningController::setNoteMapping(const TuningTableMap* mapping, bool updateTuner)
{
    mappings.add(std::make_unique<TuningTableMap>(*mapping));
    currentMapping = mappings.getLast();
    juce::Logger::writeToLog("Loaded new tuning table map.");
    if (updateTuner)
        updateCurrentTuner();
}

void MappedTuningController::setNoteMapping(const TuningTableMap* mapping)
{
    setNoteMapping(mapping, true);
}

void MappedTuningController::setTunings(const Tuning* sourceTuning, const Tuning* targetTuning, const TuningTableMap* mapping)
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
    
    auto mapping = newTuningMap(currentTuningTarget.get());
    setNoteMapping(mapping.get(), updateTuner);
}

std::unique_ptr<TuningTableMap> MappedTuningController::newTuningMap(const Tuning* tuning)
{
    return newTuningMap(tuning, mappingType);
}

std::unique_ptr<TuningTableMap> MappedTuningController::NewLinearMappingFromTuning(const Tuning* tuning)
{
    auto mapFunction = Map<int>::FunctionDefinition
    {
        2048,
        tuning->getRootIndex(),
        [&](int x) { return modulo(x - tuning->getRootIndex(), 2048); }
    };
    auto linearMap = Map<int>(mapFunction);
    auto definition = TuningTableMap::Definition
    {
        tuning->getRootMidiNote(), tuning->getRootIndex(), &linearMap
    };
   
    return std::make_unique<TuningTableMap>(definition);
}

std::unique_ptr<TuningTableMap> MappedTuningController::NewPeriodicMappingFromTuning(const Tuning* tuning)
{
    // gotta fix this keytographer factory function
    auto periodicMap = MultichannelMap::CreatePeriodicMapping(tuning->getTuningSize(), tuning->getRootMidiNote(), tuning->getRootMidiChannel());
    return std::make_unique<TuningTableMap>(periodicMap);
}

std::unique_ptr<TuningTableMap> MappedTuningController::newTuningMap(const Tuning* tuning, Multimapper::MappingType mappingType)
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