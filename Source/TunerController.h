/*
  ==============================================================================

    TunerController.h
    Created: 11 Dec 2021 9:43:20pm
    Author:  Vincenzo

    Manage tunings and mappings, and also create mappings in AutoMapping mode.
    
    Uses shared pointers to preserve MidiNoteTuner objects that are assigned
    to active notes so that Note-Off messages are still scoped to the parameters
    the associated Note-On message was created with.

  ==============================================================================
*/

#pragma once
#include "MidiNoteTuner.h"
#include "./mapping/MultichannelMap.h"
#include "Common.h"

class TunerController
{
public:

    class Watcher
    {
    public:
        virtual void sourceTuningChanged(const std::shared_ptr<MappedTuningTable>& source) {}
        virtual void targetTuningChanged(const std::shared_ptr<MappedTuningTable>& target) {}
    };

private:

    TuningTableMap::Root sourceMapRoot;
    TuningTableMap::Root targetMapRoot;

    MappedTuningTable::FrequencyReference sourceReference;
    MappedTuningTable::FrequencyReference targetReference;

    std::shared_ptr<MappedTuningTable> currentTuningSource;
    std::shared_ptr<MappedTuningTable> currentTuningTarget;

    std::shared_ptr<MidiNoteTuner> currentTuner;

    Everytone::MappingMode mappingMode = Everytone::MappingMode::Auto;
    Everytone::MappingType mappingType = Everytone::MappingType::Linear;

    int pitchbendRange = 4;

    juce::ListenerList<Watcher> watchers;

public:

    TunerController(Everytone::MappingMode mappingMode=Everytone::MappingMode::Auto, Everytone::MappingType mappingType=Everytone::MappingType::Linear);

    TunerController(std::shared_ptr<TuningTable> sourceTuning, TuningTableMap::Root sourceMapRootIn, 
                    std::shared_ptr<TuningTable> targetTuning, TuningTableMap::Root targetMapRootIn,
                    Everytone::MappingMode mappingMode, Everytone::MappingType mappingType = Everytone::MappingType::Linear);

    TunerController(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTableMap> sourceMapping,
                    std::shared_ptr<TuningTable> targetTuning, std::shared_ptr<TuningTableMap> targetMapping,
                    Everytone::MappingMode mappingMode, Everytone::MappingType mappingType = Everytone::MappingType::Linear);
    
    ~TunerController();

    std::shared_ptr<MidiNoteTuner>& getTuner() { return currentTuner; }

    TuningTableMap::Root getSourceMapRoot() const { return sourceMapRoot; }
    TuningTableMap::Root getTargetMapRoot() const { return targetMapRoot; }

    const MappedTuningTable* readTuningSource() const { return currentTuningSource.get(); }
    const MappedTuningTable* readTuningTarget() const { return currentTuningTarget.get(); }

    Everytone::MappingMode getMappingMode() const { return mappingMode; }
    Everytone::MappingType getMappingType() const { return mappingType; }

    int getPitchbendRange() const { return pitchbendRange; }

    
    void addWatcher(Watcher* watcher) { watchers.add(watcher); }
    void removeWatcher(Watcher* watcher) { watchers.remove(watcher); }
    void clearWatchers() { watchers.clear(); }


    // Tuning Setters

    void setSourceTuning(std::shared_ptr<TuningTable> tuning, bool setReference = false, MappedTuningTable::FrequencyReference reference = MappedTuningTable::FrequencyReference());
    void setSourceTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping);
    void setSourceTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping, MappedTuningTable::FrequencyReference sourceReference);

    void setTargetTuning(std::shared_ptr<TuningTable> tuning, bool setReference = false, MappedTuningTable::FrequencyReference reference = MappedTuningTable::FrequencyReference());
    void setTargetTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping);
    void setTargetTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping, MappedTuningTable::FrequencyReference targetReference);

    void setTunings(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTable> targetTuning);
    void setTunings(std::shared_ptr<TuningTable> sourceTuning, MappedTuningTable::FrequencyReference sourceReference,
                    std::shared_ptr<TuningTable> targetTuning, MappedTuningTable::FrequencyReference targetReference);


    void setTunings(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTableMap> sourceMapping,
                    std::shared_ptr<TuningTable> targetTuning, std::shared_ptr<TuningTableMap> targetMapping);

    void setTunings(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTableMap> sourceMapping, MappedTuningTable::FrequencyReference sourceReference,
                    std::shared_ptr<TuningTable> targetTuning, std::shared_ptr<TuningTableMap> targetMapping, MappedTuningTable::FrequencyReference targetReference);


    // Mutators

    void remapSource(const TuningTableMap::Definition& mapDefinition);
    void remapTarget(const TuningTableMap::Definition& mapDefinition);

    void setSourceReference(MappedTuningTable::FrequencyReference reference);
    void setTargetReference(MappedTuningTable::FrequencyReference reference);

    void setSourceRootFrequency(double frequency);
    void setTargetRootFrequency(double frequency);

    void setSourceMapRoot(TuningTableMap::Root root);
    void setTargetMapRoot(TuningTableMap::Root root);

    void setSourceMappedTuningRoot(MappedTuningTable::Root root);
    void setTargetMappedTuningRoot(MappedTuningTable::Root root);


    // Options

    void setMappingMode(Everytone::MappingMode mode);
    void setMappingType(Everytone::MappingType type);

    void setPitchbendRange(int pitchbendRange);

private:

    std::shared_ptr<TuningTableMap> mapForTuning(const TuningTable* tuning, bool isTarget);

    void setSourceTuning(std::shared_ptr<TuningTable>& tuning, std::shared_ptr<TuningTableMap>& mapping, bool updateTuner);
    void setSource(std::shared_ptr<MappedTuningTable>& mappedTuning, bool updateTuner);

    void setTargetTuning(std::shared_ptr<TuningTable>& tuning, std::shared_ptr<TuningTableMap>& mapping, bool updateTuner);
    void setTarget(std::shared_ptr<MappedTuningTable>& mappedTuning, bool updateTuner);

    void setTunings(std::shared_ptr<TuningTable>& sourceTuning, std::shared_ptr<TuningTableMap>& sourceMapping,
                    std::shared_ptr<TuningTable>& targetTuning, std::shared_ptr<TuningTableMap>& targetMapping, bool sendChangeMessages);

    void updateCurrentTuner();

    void mappingTypeChanged();

public:

    // For use in "Auto Mapping" mode with tunings created in the app
    static std::shared_ptr<TuningTableMap> NewMappingFromTuning(const TuningTable* tuningDefinition, TuningTableMap::Root root, Everytone::MappingType mappingType);
};

