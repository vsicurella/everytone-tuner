/*
  ==============================================================================

    TunerController.h
    Created: 11 Dec 2021 9:43:20pm
    Author:  Vincenzo

    Manage tunings and mappings, and also create mappings in AutoMapping mode.
    It should only allocate data when necessary by using shared pointers.

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
        virtual void sourceTuningChanged(const std::shared_ptr<MappedTuning>& source) {}
        virtual void targetTuningChanged(const std::shared_ptr<MappedTuning>& target) {}
    };

private:

    TuningTableMap::Root sourceMapRoot;
    TuningTableMap::Root targetMapRoot;

    std::shared_ptr<MappedTuning> currentTuningSource;
    std::shared_ptr<MappedTuning> currentTuningTarget;

    std::shared_ptr<MidiNoteTuner> currentTuner;

    Everytone::MappingMode mappingMode = Everytone::MappingMode::Auto;
    Everytone::MappingType mappingType = Everytone::MappingType::Linear;

    int pitchbendRange = 4;

    juce::ListenerList<Watcher> watchers;

public:

    TunerController(Everytone::MappingMode mappingMode, Everytone::MappingType mappingType=Everytone::MappingType::Linear);

    TunerController(CentsDefinition sourceTuning, TuningTableMap::Root sourceMapRootIn, 
                    CentsDefinition targetTuning, TuningTableMap::Root targetMapRootIn,
                    Everytone::MappingMode mappingMode, Everytone::MappingType mappingType = Everytone::MappingType::Linear);

    TunerController(CentsDefinition sourceTuning, TuningTableMap::Definition sourceMapping,
                    CentsDefinition targetTuning, TuningTableMap::Definition targetMapping,
                    Everytone::MappingMode mappingMode, Everytone::MappingType mappingType = Everytone::MappingType::Linear);
    
    ~TunerController();

    std::shared_ptr<MidiNoteTuner>& getTuner() { return currentTuner; }

    TuningTableMap::Root getSourceMapRoot() const { return sourceMapRoot; }
    TuningTableMap::Root getTargetMapRoot() const { return targetMapRoot; }

    const MappedTuning* readTuningSource() const { return currentTuningSource.get(); }
    const MappedTuning* readTuningTarget() const { return currentTuningTarget.get(); }

    Everytone::MappingMode getMappingMode() const { return mappingMode; }
    Everytone::MappingType getMappingType() const { return mappingType; }

    int getPitchbendRange() const { return pitchbendRange; }

    
    void addWatcher(Watcher* watcher) { watchers.add(watcher); }
    void removeWatcher(Watcher* watcher) { watchers.remove(watcher); }
    void clearWatchers() { watchers.clear(); }


    void loadSourceTuning(const CentsDefinition& tuningDefinition);
    //void loadSourceTuning(const CentsDefinition& tuningDefinition, const TuningTableMap::Definition& mapDefinition);

    void loadTargetTuning(const CentsDefinition& tuningDefinition);
    //void loadTargetTuning(const CentsDefinition& tuningDefinition, const TuningTableMap::Definition& mapDefinition);

    void remapSource(const TuningTableMap::Definition& mapDefinition);
    void remapTarget(const TuningTableMap::Definition& mapDefinition);

    void setSourceRootFrequency(double frequency);
    void setTargetRootFrequency(double frequency);

    void setSourceMapRoot(TuningTableMap::Root root);
    void setTargetMapRoot(TuningTableMap::Root root);

    void setSourceMappedTuningRoot(MappedTuning::Root root);
    void setTargetMappedTuningRoot(MappedTuning::Root root);

    void loadTunings(const CentsDefinition& sourceTuningDefinition, const CentsDefinition& targetTuningDefinition);
    //void setTunings(const CentsDefinition& sourceTuningDefinition, const CentsDefinition& targetTuningDefinition,
    //                const TuningTableMap::Definition& sourceMapDefinition, const TuningTableMap::Definition& targetMapDefinition);

    void setMappingMode(Everytone::MappingMode mode);
    void setMappingType(Everytone::MappingType type);

    void setPitchbendRange(int pitchbendRange);

private:

    std::shared_ptr<TuningTableMap> mapForTuning(const Tuning* tuning, bool isTarget);

    void loadSourceTuning(const CentsDefinition& definition, bool updateTuner);
    void loadTargetTuning(const CentsDefinition& definition, bool updateTuner);

    void setSourceTuning(std::shared_ptr<Tuning> tuning, std::shared_ptr<TuningTableMap> mapping, bool updateTuner = true);
    void setTargetTuning(std::shared_ptr<Tuning> tuning, std::shared_ptr<TuningTableMap> mapping, bool updateTuner = true);

    void setTunings(std::shared_ptr<Tuning> sourceTuning, std::shared_ptr<TuningTableMap> sourceMapping,
                    std::shared_ptr<Tuning> targetTuning, std::shared_ptr<TuningTableMap> targetMapping);

    void updateCurrentTuner();

    void mappingTypeChanged();

public:

    // For use in "Auto Mapping" mode with tunings created in the app
    static std::shared_ptr<TuningTableMap> NewLinearMappingFromTuning(const Tuning* tuningDefinition, TuningTableMap::Root root);
    static std::shared_ptr<TuningTableMap> NewPeriodicMappingFromTuning(const Tuning* tuningDefinition, TuningTableMap::Root root);

    static std::shared_ptr<TuningTableMap> NewMappingFromTuning(const Tuning* tuningDefinition, TuningTableMap::Root root, Everytone::MappingType mappingType);
};

