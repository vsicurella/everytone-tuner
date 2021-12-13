/*
  ==============================================================================

    MappingChanger.h
    Created: 12 Dec 2021 5:51:03pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "MultimapperCommon.h"
#include <JuceHeader.h>

struct MappingChanger;

struct MappingWatcher
{
    virtual void mappingTypeChanged(MappingChanger* changer, Multimapper::MappingType type) {};
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