/*
  ==============================================================================

    VoiceBank.h
    Created: 4 Apr 2022 7:09:21pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "../Common.h"
#include "MidiVoice.h"

#define MAX_CHANNELS 16
#define MAX_VOICES_PER_CHANNEL 128
#define MAX_VOICES MAX_CHANNELS * MAX_VOICES_PER_CHANNEL

class VoiceBank
{
public:

    struct VoicePtr
    {
        MidiVoice voice;
        bool locked = false;

        void setLocked(bool lock)
        {
            locked = lock;
        }
    };

    struct ChannelInfo
    {
        int id = 0;
        int voiceLimit = MAX_VOICES_PER_CHANNEL;
        VoicePtr* channelPtr = nullptr;
        bool disabled = false;
        int numVoices = 0;

        ChannelInfo() {}

        ChannelInfo(int idIn, int voiceLimitIn, VoicePtr* firstVoice, bool disabledIn = false)
            : id(idIn), voiceLimit(voiceLimitIn), channelPtr(firstVoice), disabled(disabledIn) {}

        bool isFull() const { return voiceLimit == numVoices; }

        VoicePtr* getVoicePtr(int index) 
        { 
            jassert(channelPtr != nullptr);
            jassert(index >= 0);
            return &channelPtr[index]; 
        }
    };

    enum class NewVoiceState
    {
        Normal = 0, // Not all voices are used - get next channel
        Overflow,   // All voices are used, determine action via NotePriority
        Monophonic  // One voice at a time - continue using same channel
    };

private:

    //struct MetaVoice
    //{
    //    int channelIndex = -1;
    //    int voiceIndex = -1;
    //    VoicePtr& voice;
    //};

private:

    const int maxVoiceLimit = MAX_VOICES;

    int voicesSize = 0;
    VoicePtr voices[MAX_VOICES];
    int numVoices = 0;
    int voiceLimit = maxVoiceLimit;

    ChannelInfo stolenChannelInfo;
    VoicePtr stolenVoices[MAX_VOICES_PER_CHANNEL];

    juce::Array<ChannelInfo> channelInfo;

    // Sorted by input note number, then midi channel (0 being stolen)
    int noteMapSize = 0;
    int mapIndicesPerNote = 0;
    VoicePtr* inputNoteMap[MAX_VOICES + MAX_VOICES_PER_CHANNEL];

    Everytone::ChannelMode channelMode = Everytone::ChannelMode::FirstAvailable;
    Everytone::MpeZone mpeZone = Everytone::MpeZone::Lower;
    Everytone::NotePriority notePriority = Everytone::NotePriority::Last;

    int lastChannelAssigned = 0;

    int nextVoiceIndex = 0;

private:

    void reset();

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

    int getMapNoteIndex(int midiNote) const;

    int indexOfVoice(int midiChannel, int midiNote) const;
    int indexOfVoice(const MidiVoice* voice) const;

    int effectiveVoiceLimit() const;
    void updateVoiceLimitCache();
    int numVoicesAvailable() const;

    const VoicePtr* getExistingVoice(int index) const;

    void setVoiceInChannel(ChannelInfo chInfo, int index, MidiVoice& voice);
    void setVoiceInChannel(int midiChannel, int index, MidiVoice& voice);

    // Returns the index in the channel
    int addVoiceToChannel(ChannelInfo& chInfo, MidiVoice& voice);
    int addVoiceToChannel(int midiChannel, MidiVoice& voice);

    // Returns the index the voice was in
    int removeVoiceFromChannel(ChannelInfo& chInfo, MidiVoice& voice);
    int removeVoiceFromChannel(int midiChannel, MidiVoice& voice);

    void stealExistingVoice(int index);
    void retriggerExistingVoice(int index, int midiChannel);

    MidiVoice removeVoice(int index);

public:

    VoiceBank(
        Everytone::ChannelMode channelmodeIn = Everytone::ChannelMode::FirstAvailable,
        Everytone::MpeZone mpeZone = Everytone::MpeZone::Lower);
    ~VoiceBank();

    Everytone::ChannelMode getChannelMode() const { return channelMode; }
    Everytone::MpeZone getMpeZone() const { return mpeZone; }
    Everytone::NotePriority getNotePriority() const { return notePriority; }

    int getVoiceLimit() const { return voiceLimit; }

    // Number of all voices held, regardless if they are active
    int numAllVoices() const;

    // Array of all voices held, regardless if they are active
    juce::Array<MidiVoice> getAllVoices() const;

    // Returns a copy of the array of pointers to MidiVoices that are in the given channel
    juce::Array<MidiVoice> getVoicesInChannel(int midiChannel) const;

    int numActiveVoices() const;


    const MidiVoice* findVoice(MidiVoice& voiceToFind);

    const MidiVoice* findChannelAndAddVoice(MidiVoice& newVoice);


    const MidiVoice* getVoice(MidiVoice& voice);
    const MidiVoice* getVoice(const juce::MidiMessage& msg);

    MidiVoice removeVoice(int midiChannel, int midiNote);
    MidiVoice removeVoice(const juce::MidiMessage& msg);
    MidiVoice removeVoice(const MidiVoice* voice);

    void clearAllVoices();

    //const MidiVoice* getVoiceWithPitch(MidiPitch pitch) const;

    int channelOfVoice(int midiChannel, int midiNote) const;
    int channelOfVoice(MidiVoice* voice) const;
    int channelOfVoice(const juce::MidiMessage& msg) const;

    bool channelIsFree(int midiChannel, MidiPitch pitchToAssign = MidiPitch()) const;

    void setChannelMode(Everytone::ChannelMode mode);
    void setMpeZone(Everytone::MpeZone zone);
    void setNotePriority(Everytone::NotePriority notePriorityIn);
    void setChannelsDisabled(juce::Array<bool> channelsDisabledIn);
};