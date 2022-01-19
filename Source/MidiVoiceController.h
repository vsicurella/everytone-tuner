/*
  ==============================================================================

    MidiVoiceController.h
    Created: 12 Dec 2021 2:14:00pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include "TunerController.h"
#include "MidiVoice.h"

#define EVERYTONE_LOG_VOICES 0

class MidiVoiceController
{
public:

    class Watcher
    {
    public:

        virtual void voiceAdded(MidiVoice voice) {}
        virtual void voiceChanged(MidiVoice voice) {}
        virtual void voiceRemoved(MidiVoice voice) {}
    };

private:

    juce::ListenerList<MidiVoiceController::Watcher> voiceWatchers;
    
public:
    void addVoiceWatcher(MidiVoiceController::Watcher* watcherIn) { voiceWatchers.add(watcherIn); }
    void removeVoiceWatcher(MidiVoiceController::Watcher* watcherIn) { voiceWatchers.remove(watcherIn); }

private:

    enum class NewVoiceState
    {
        Normal = 0, // Not all voices are used - get next channel
        Overflow,   // All voices are used, determine action via NotePriority
        Monophonic  // One voice at a time - continue using same channel
    };


private:

    TunerController& tuningController;

    int maxVoiceLimit = 16;

    juce::OwnedArray<MidiVoice> voices;
    juce::Array<juce::Array<MidiVoice*>> voicesPerChannel;
    juce::Array<MidiVoice*> activeVoices;

    juce::Array<bool> midiChannelDisabled;

    MidiBuffer notePriorityQueue;
    int notePrioritySample = 0;

    Everytone::ChannelMode channelMode = Everytone::ChannelMode::FirstAvailable;
    Everytone::MpeZone mpeZone = Everytone::MpeZone::Lower;
    Everytone::NotePriority notePriority = Everytone::NotePriority::Last;

    int voiceLimit = maxVoiceLimit;

    int lastChannelAssigned = 0;

private:

    void resetVoicesPerChannel();

    NewVoiceState getNewVoiceState() const;

    int findLowestVoiceIndex(bool active) const;
    int findHighestVoiceIndex(bool active) const;
    int findOldestVoiceIndex(bool active) const;
    int findMostRecentVoiceIndex(bool active) const;

    int nextAvailableChannel() const;
    int nextRoundRobinChannel() const;

    int getNextVoiceIndexToSteal() const;
    int getNextVoiceToRetrigger() const;

    int findNextVoiceChannel(MidiPitch pitchOfVoice = MidiPitch()) const;

    int midiNoteIndex(int midiChannel, int midiNote) const;

    int indexOfVoice(int midiChannel, int midiNote) const;
    int indexOfVoice(const MidiVoice* voice) const;

    int effectiveVoiceLimit() const;

    const MidiVoice* getExistingVoice(int index) const;

    void queueVoiceNoteOff(MidiVoice* voice);
    void queueVoiceNoteOn(MidiVoice* voice, bool pitchbendOnly=false);
    
    void addVoiceToChannel(int midiChannel, MidiVoice* voice);
    void removeVoiceFromChannel(int midiChannel, MidiVoice* voice);
    
    void stealExistingVoice(int index);
    void retriggerExistingVoice(int index, int midiChannel);

    const MidiVoice* findChannelAndAddVoice(int midiChannel, int midiNote, juce::uint8 velocity);    
    MidiVoice removeVoice(int index);

public:

    MidiVoiceController(TunerController& tuningController, 
                        Everytone::ChannelMode channelmodeIn = Everytone::ChannelMode::FirstAvailable,
                        Everytone::MpeZone mpeZone = Everytone::MpeZone::Lower);
    ~MidiVoiceController();

    Everytone::ChannelMode getChannelMode() const { return channelMode; }
    Everytone::MpeZone getMpeZone() const { return mpeZone; }
    Everytone::NotePriority getNotePriority() const { return notePriority; }

    int getVoiceLimit() const { return voiceLimit; }

    // Number of all voices held, regardless if they are active
    int numAllVoices() const;

    // Array of all voices held, regardless if they are active
    juce::Array<MidiVoice> getAllVoices() const;

    // Returns a copy of the array of pointers to MidiVoices that are in the given channel
    juce::Array<MidiVoice*> getVoicesInChannel(int midiChannel) const;

    int numActiveVoices() const;

    const MidiVoice* getVoice(int midiChannel, int midiNote, juce::uint8 velocity = 0);
    const MidiVoice* getVoice(const juce::MidiMessage& msg);

    MidiVoice removeVoice(int midiChannel, int midiNote);
    MidiVoice removeVoice(const juce::MidiMessage& msg);
    MidiVoice removeVoice(const MidiVoice* voice);

    void clearAllVoices();

    // Get messages queued for NotePriority settings, returns size of buffer
    int serveNotePriorityMessages(MidiBuffer& queueOut);

    //const MidiVoice* getVoiceWithPitch(MidiPitch pitch) const;

    int channelOfVoice(int midiChannel, int midiNote) const;
    int channelOfVoice(MidiVoice* voice) const;
    int channelOfVoice(const juce::MidiMessage& msg) const;

    bool channelIsFree(int midiChannel, MidiPitch pitchToAssign = MidiPitch()) const;

    void setChannelDisabled(int midiChannel, bool disabled);
    
    void setChannelMode(Everytone::ChannelMode mode);
    void setMpeZone(Everytone::MpeZone zone);
    void setNotePriority(Everytone::NotePriority notePriorityIn);
    void setVoiceLimit(int voiceLimit);
};