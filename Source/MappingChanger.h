/*
  ==============================================================================

    MappingChanger.h
    Created: 12 Dec 2021 5:51:03pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "Common.h"
#include <JuceHeader.h>

struct MappingChanger;

struct MappingWatcher
{
    virtual void mappingRootChanged(int rootMidiChannel, int rootMidiNote) {};
};

class MappingChanger
{
protected:
    juce::ListenerList<MappingWatcher> mappingWatchers;

public:
    virtual ~MappingChanger() { mappingWatchers.clear(); }

    void addMappingWatcher(MappingWatcher* watcher) { mappingWatchers.add(watcher); }
    void removeMappingWatcher(MappingWatcher* watcher) { mappingWatchers.remove(watcher); }
};