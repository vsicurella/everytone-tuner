/*
  ==============================================================================

    TuningMapHelper.h
    Created: 11 Dec 2021 9:43:20pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Tuning.h"
#include "TuningTableMap.h"
#include "MultichannelMap.h"

class TuningMapHelper
{
public:

    enum class MappingType
    {
        Linear,
        Periodic,
        Custom
    };

private:

    MappingType mappingType = MappingType::Linear;

    std::unique_ptr<Keytographer::TuningTableMap> map;

public:

    TuningMapHelper();

    ~TuningMapHelper();

    const Keytographer::TuningTableMap& getTuningMap() const { return *map; }

    void createTuningMap(const Tuning& tuning);

    void setMappingType(MappingType type, const Tuning* = nullptr);

private:

    static Keytographer::TuningTableMap* NewLinearMappingFromTuning(const Tuning& tuning);
    static Keytographer::TuningTableMap* NewPeriodicMappingFromTuning(const Tuning& tuning);
};