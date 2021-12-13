/*
  ==============================================================================

    MappedTuningController.h
    Created: 11 Dec 2021 9:43:20pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "MidiNoteTuner.h"
#include "MultichannelMap.h"
#include "MultimapperCommon.h"

class MappedTuningController
{
    juce::Array<std::shared_ptr<Tuning>> tunings;
    std::shared_ptr<Tuning> currentTuningSource;
    std::shared_ptr<Tuning> currentTuningTarget;

    juce::Array<std::shared_ptr<Keytographer::TuningTableMap>> mappings;
    std::shared_ptr<Keytographer::TuningTableMap> currentMapping;

    juce::Array<std::shared_ptr<MidiNoteTuner>> tuners;
    std::shared_ptr<MidiNoteTuner> currentTuner;

    Multimapper::MappingMode mappingMode = Multimapper::MappingMode::Auto;
    Multimapper::MappingType mappingType = Multimapper::MappingType::Linear;

public:

    MappedTuningController();

    ~MappedTuningController();

    const Tuning* getTuningSource() const { return currentTuningSource.get(); }

    const Tuning* getTuningTarget() const { return currentTuningTarget.get(); }

    const Keytographer::TuningTableMap* getMapping() const { return currentMapping.get(); }


    void setSourceTuning(const Tuning* tuning);
    
    void setTargetTuning(const Tuning* tuning);

    void setNoteMapping(const Keytographer::TuningTableMap* mapping);

    void setTunings(const Tuning* sourceTuning, const Tuning* targetTuning, const Keytographer::TuningTableMap* mapping=nullptr);
   
    void setMappingType(Multimapper::MappingType type);


private:

    void updateAutoMapping(bool updateTuner);

    void setSourceTuning(const Tuning* tuning, bool updateTuner);

    void setTargetTuning(const Tuning* tuning, bool updateTuner);

    void setNoteMapping(const Keytographer::TuningTableMap* mapping, bool updateTuner);

    void updateCurrentTuner();

    Keytographer::TuningTableMap* newTuningMap(const Tuning* tuning);

public:

    static Keytographer::TuningTableMap* newTuningMap(const Tuning* tuning, Multimapper::MappingType mappingType);

    static Keytographer::TuningTableMap* NewLinearMappingFromTuning(const Tuning* tuning);

    static Keytographer::TuningTableMap* NewPeriodicMappingFromTuning(const Tuning* tuning);
};

