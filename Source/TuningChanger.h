/*
  ==============================================================================

    TuningBroadcaster.h
    Created: 9 Dec 2021 8:55:10pm
    Author:  Vincenzo

  ==============================================================================
*/
    
#pragma once

#include <JuceHeader.h>
#include "Tuning.h"

class TuningChanger;

class TuningWatcher
{
public:
    virtual void tuningSourceChanged(TuningChanger* changer, const Tuning* newTuning) {}
    virtual void tuningTargetChanged(TuningChanger* changer, const Tuning* newTuning) {}
    virtual void tuningSourceReferenceChanged(TuningChanger* changer, Tuning::Reference reference) {}
    virtual void tuningTargetReferenceChanged(TuningChanger* changer, Tuning::Reference reference) {}
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