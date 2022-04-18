/*
  ==============================================================================

    VoiceWatcher.h
    Created: 17 Apr 2022 6:34:33pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "MidiVoice.h"

class VoiceWatcher
{
public:

    virtual ~VoiceWatcher() {}

    virtual void voiceAdded(MidiVoice voice) {}
    virtual void voiceChanged(MidiVoice voice) {}
    virtual void voiceRemoved(MidiVoice voice) {}
};

class VoiceChanger
{
protected:

    juce::ListenerList<VoiceWatcher> voiceWatchers;

public:

    virtual ~VoiceChanger() {}

    void addVoiceWatcher(VoiceWatcher* watcherIn) { voiceWatchers.add(watcherIn); }
    void removeVoiceWatcher(VoiceWatcher* watcherIn) { voiceWatchers.remove(watcherIn); }
};
