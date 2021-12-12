/*
  ==============================================================================

    TuningMapHelper.cpp
    Created: 11 Dec 2021 9:43:20pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TuningMapHelper.h"

TuningMapHelper::TuningMapHelper()
    : map(std::make_unique<Keytographer::TuningTableMap>(Keytographer::MultichannelMap::StandardMapping()))
{
}

TuningMapHelper::~TuningMapHelper()
{

}

void TuningMapHelper::createTuningMap(const Tuning& tuning)
{
    switch (mappingType)
    {
    case MappingType::Linear:
    {
        map.reset(NewLinearMappingFromTuning(tuning));
        break;
    }

    case MappingType::Periodic:
    {
        map.reset(NewPeriodicMappingFromTuning(tuning));
        break;
    }


    case MappingType::Custom:

        break;


    default:
        jassertfalse;
    }
}

void TuningMapHelper::setMappingType(MappingType type, const Tuning* tuning)
{
    mappingType = type;

    if (tuning != nullptr)
        createTuningMap(*tuning);
}

Keytographer::TuningTableMap* TuningMapHelper::NewLinearMappingFromTuning(const Tuning& tuning)
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

Keytographer::TuningTableMap* TuningMapHelper::NewPeriodicMappingFromTuning(const Tuning& tuning)
{
    auto periodicMap = Keytographer::MultichannelMap::CreatePeriodicMapping(tuning.getTuningSize(), tuning.getRootMidiNote(), tuning.getRootMidiChannel());
    auto map = new Keytographer::TuningTableMap(periodicMap);
    return map;
}