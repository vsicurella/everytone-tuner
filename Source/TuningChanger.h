/*
  ==============================================================================

    TuningBroadcaster.h
    Created: 9 Dec 2021 8:55:10pm
    Author:  Vincenzo

  ==============================================================================
*/
    
#pragma once

#include <JuceHeader.h>
#include "./tuning/Tuning.h"
#include "./mapping/TuningTableMap.h"

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

    // Used when only the tuning's root frequency changes - mapping stays the same in either Auto or Manual mode
    virtual void sourceRootFrequencyChanged(TuningChanger* changer, double frequency) {}
    virtual void targetRootFrequencyChanged(TuningChanger* changer, double frequency) {}
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