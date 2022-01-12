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

#define MULTIMAPPER_MAX_VOICES 16

class MidiVoiceController
{
public:

    class Watcher
    {
    public:

        virtual void voiceAdded(MidiVoice* voice) {}
        virtual void voiceChanged(MidiVoice* voice) {}
        virtual void voiceRemoved(MidiVoice* voice) {}
    };


private:
    TunerController& tuningController;

    juce::OwnedArray<MidiVoice> voices;
    juce::Array<MidiVoice*> activeVoices;

    juce::Array<bool> midiChannelDisabled;

    MidiBuffer inactiveVoiceMessages;

    Everytone::ChannelMode channelMode = Everytone::ChannelMode::FirstAvailable;
    Everytone::MpeZone mpeZone = Everytone::MpeZone::Lower;
    Everytone::NotePriority notePriority = Everytone::NotePriority::Last;

    int voiceLimit = MULTIMAPPER_MAX_VOICES;

    int currentVoices = 0;
    int lastChannelAssigned = 0;

private:

    int findLowestVoiceIndex(bool active) const;
    int findHighestVoiceIndex(bool active) const;
    int findOldestVoice(bool active) const;
    int findMostRecentVoice(bool active) const;

    int nextAvailableVoiceIndex() const;
    int nextRoundRobinVoiceIndex() const;
    int getNextVoiceIndex() const;

    int midiNoteIndex(int midiChannel, int midiNote) const;

    int indexOfVoice(int midiChannel, int midiNote) const;
    int indexOfVoice(const MidiVoice* voice) const;

    int effectiveVoiceLimit() const;

    const MidiVoice* addVoice(int midiChannel, int midiNote, juce::uint8 velocity);
    const MidiVoice* getExistingVoice(int index) const;
    MidiVoice removeVoice(int index);

public:

    MidiVoiceController(TunerController& tuningController, 
                        Everytone::ChannelMode channelmodeIn = Everytone::ChannelMode::FirstAvailable,
                        Everytone::MpeZone mpeZone = Everytone::MpeZone::Lower,
                        int voiceLimit = MULTIMAPPER_MAX_VOICES);
    ~MidiVoiceController();

    Everytone::ChannelMode getChannelMode() const { return channelMode; }
    Everytone::MpeZone getMpeZone() const { return mpeZone; }
    Everytone::NotePriority getNotePriority() const { return notePriority; }

    int getVoiceLimit() const { return voiceLimit; }

    // Number of all voices held, regardless if they are active
    int numAllVoices() const;

    // Array of all voices held, regardless if they are active
    juce::Array<MidiVoice> getAllVoices() const;

    int numActiveVoices() const;

    const MidiVoice* getVoice(int midiChannel, int midiNote, juce::uint8 velocity = 0);
    const MidiVoice* getVoice(const juce::MidiMessage& msg);

    MidiVoice removeVoice(int midiChannel, int midiNote);
    MidiVoice removeVoice(const juce::MidiMessage& msg);
    MidiVoice removeVoice(const MidiVoice* voice);

    //const MidiVoice* getVoiceWithPitch(MidiPitch pitch) const;

    int channelOfVoice(int midiChannel, int midiNote) const;
    int channelOfVoice(const juce::MidiMessage& msg) const;

    bool channelIsFree(int channelNumber, MidiPitch pitchToAssign = MidiPitch()) const;

    void setChannelDisabled(int midiChannel, bool disabled);
    
    void setChannelMode(Everytone::ChannelMode mode);
    void setMpeZone(Everytone::MpeZone zone);
    void setNotePriority(Everytone::NotePriority notePriorityIn);
    void setVoiceLimit(int voiceLimit);

};