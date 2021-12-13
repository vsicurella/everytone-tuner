/*
  ==============================================================================

    MappedTuningController.h
    Created: 11 Dec 2021 9:43:20pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Tuning.h"
#include "TuningTableMap.h"
#include "MultichannelMap.h"
#include "MultimapperCommon.h"

class MappedTuningController
{

private:

    Multimapper::MappingType mappingType = Multimapper::MappingType::Linear;

public:

    MappedTuningController();

    ~MappedTuningController();

    void commitAutoMapping();

    void setMappingType(Multimapper::MappingType type, const Tuning* = nullptr);

public:

    static Keytographer::TuningTableMap* newTuningMap(const Tuning& tuning, Multimapper::MappingType mappingType);

private:

    static Keytographer::TuningTableMap* NewLinearMappingFromTuning(const Tuning& tuning);
    static Keytographer::TuningTableMap* NewPeriodicMappingFromTuning(const Tuning& tuning);
};

