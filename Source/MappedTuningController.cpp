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
}

MappedTuningController::~MappedTuningController()
{

}

void MappedTuningController::commitAutoMapping()
{

}

void MappedTuningController::setMappingType(Multimapper::MappingType type, const Tuning* tuning)
{
    mappingType = type;
}

Keytographer::TuningTableMap* MappedTuningController::NewLinearMappingFromTuning(const Tuning& tuning)
{
    auto mapFunction = Keytographer::Map<int>::FunctionDefinition
    {
        2048,
        tuning.getRootIndex(),
        [&](int x) { return modulo(x - tuning.getRootIndex(), 2048); }
    };
    auto linearMap = Keytographer::Map<int>(mapFunction);
    auto definition = Keytographer::TuningTableMap::Definition
    {
        tuning.getRootMidiNote(), tuning.getRootIndex(), &linearMap
    };
   
    auto map = new Keytographer::TuningTableMap(definition);
    return map;
}

Keytographer::TuningTableMap* MappedTuningController::NewPeriodicMappingFromTuning(const Tuning& tuning)
{
    auto periodicMap = Keytographer::MultichannelMap::CreatePeriodicMapping(tuning.getTuningSize(), tuning.getRootMidiNote(), tuning.getRootMidiChannel());
    auto map = new Keytographer::TuningTableMap(periodicMap);
    return map;
}

Keytographer::TuningTableMap* MappedTuningController::newTuningMap(const Tuning& tuning, Multimapper::MappingType mappingType)
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


    case Multimapper::MappingType::Custom:

        break;


    default:
        jassertfalse;
    }

    return nullptr;
}