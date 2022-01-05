/*
  ==============================================================================

    TuningChanger.h
    Created: 9 Dec 2021 8:55:10pm
    Author:  Vincenzo

  ==============================================================================
*/
    
#pragma once

#include "./tuning/MappedTuning.h"
#include "./Common.h"

class TuningChanger;

class TuningWatcher
{
public:

    // Used when a new tuning is loaded in Auto-Mapping mode
    virtual void sourceDefinitionLoaded(TuningChanger* changer, CentsDefinition definition) {}
    virtual void targetDefinitionLoaded(TuningChanger* changer, CentsDefinition definition) {}

    // Used with a new tuning in Manual-Mapping mode
    virtual void sourceMappedTuningLoaded(TuningChanger* changer, CentsDefinition tuningDefinition, TuningTableMap::Definition mapDefinition) {}
    virtual void targetMappedTuningLoaded(TuningChanger* changer, CentsDefinition tuningDefinition, TuningTableMap::Definition mapDefinition) {}
    
    // Used when a tuning note reference changed
    virtual void sourceTuningReferenceChanged(TuningChanger* changer, MappedTuningTable::FrequencyReference reference) {}
    virtual void targetTuningReferenceChanged(TuningChanger* changer, MappedTuningTable::FrequencyReference reference) {}

    // Used when only the tuning's root frequency changes - mapping stays the same in either Auto or Manual mode
    virtual void sourceRootFrequencyChanged(TuningChanger* changer, double frequency) {}
    virtual void targetRootFrequencyChanged(TuningChanger* changer, double frequency) {}

    // Used when only the TuningTableMap midi root changes
    virtual void sourceMappingRootChanged(TuningChanger* changer, TuningTableMap::Root root) {};
    virtual void targetMappingRootChanged(TuningChanger* changer, TuningTableMap::Root root) {};

    // Used to update all references
    virtual void sourceMappedTuningRootChanged(TuningChanger* changer, MappedTuningTable::Root root) {};
    virtual void targetMappedTuningRootChanged(TuningChanger* changer, MappedTuningTable::Root root) {};

};

class TuningChanger
{

protected:
    juce::ListenerList<TuningWatcher> tuningWatchers;

public:

    TuningChanger() {}

    virtual ~TuningChanger() { tuningWatchers.clear(); }

    void addTuningWatcher(TuningWatcher* watcher) { tuningWatchers.add(watcher); }

    void removeTuningWatcher(TuningWatcher* watcher) { tuningWatchers.remove(watcher); }
};