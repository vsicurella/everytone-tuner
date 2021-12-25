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

class TuningChanger;

class TuningWatcher
{
public:
    virtual void tuningSourceChanged(TuningChanger* changer, const Tuning* newTuning) {}
    virtual void tuningTargetChanged(TuningChanger* changer, const Tuning* newTuning) {}
    virtual void tuningSourceRootFrequency(TuningChanger* changer, double frequency) {}
    virtual void tuningTargetRootFrequency(TuningChanger* changer, double frequency) {}
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