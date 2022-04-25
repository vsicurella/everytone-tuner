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

        bool isValid() const { return channelPtr != nullptr; }

        VoicePtr* getVoicePtr(int index) const
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

    struct ChannelVoicePtr
    {
        const VoicePtr* voicePtr = nullptr;
        const ChannelInfo* channel = nullptr;
        int indexInChannel = -1;

        bool isValid() const { return voicePtr != nullptr && channel != nullptr; }
        bool isInvalid() const { return voicePtr == nullptr || channel == nullptr; }

        int getVoiceIndex() const
        {
            if (channel->isValid())
                return (channel->id - 1) * MAX_VOICES_PER_CHANNEL + indexInChannel;
            return -1;
        }
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

    std::shared_ptr<MidiNoteTuner> tuner;

    // Sorted by input note number, then midi channel (0 being stolen)
    int noteMapSize = 0;
    int mapIndicesPerNote = 0;
    ChannelVoicePtr inputNoteVoiceIndexMap[MAX_VOICES];

    Everytone::ChannelMode channelMode = Everytone::ChannelMode::FirstAvailable;
    Everytone::MpeZone mpeZone = Everytone::MpeZone::Lower;
    Everytone::NotePriority notePriority = Everytone::NotePriority::Last;

    int lastChannelAssigned = 0;

    int nextVoiceIndex = 0;

private:

    void reset();

    NewVoiceState getNewVoiceState() const;

    ChannelVoicePtr findLowestVoiceIndex(bool active) const;
    ChannelVoicePtr findHighestVoiceIndex(bool active) const;
    ChannelVoicePtr findOldestVoiceIndex(bool active) const;
    ChannelVoicePtr findMostRecentVoiceIndex(bool active) const;

    int nextAvailableChannel() const;
    int nextRoundRobinChannel() const;

    ChannelVoicePtr findNextVoiceToSteal() const;
    ChannelVoicePtr findNextVoiceToRetrigger() const;

    int findNextVoiceChannel(NewVoiceState state, MidiPitch pitchOfVoice = MidiPitch()) const;

    int midiNoteIndex(int midiChannel, int midiNote) const;

    int getMapNoteIndex(int midiNote) const;
    ChannelVoicePtr getVoiceFromInputMap(int midiChannel, int midiNote) const;
    void setInputMapVoice(const ChannelVoicePtr& chVoicePtr);

    //int indexOfVoice(int midiChannel, int midiNote) const;
    //int indexOfVoice(const MidiVoice* voice) const;

    int effectiveVoiceLimit() const;
    void updateVoiceLimitCache();
    int numVoicesAvailable() const;

    const MidiVoice* getExistingVoice(int index) const;
    const VoicePtr*  getExistingVoicePtr(int index) const;

    void setVoiceInChannel(ChannelInfo& chInfo, int index, MidiVoice& voice);
    void setVoiceInChannel(int midiChannel, int index, MidiVoice& voice);

    // Returns the index in the channel
    int addVoiceToChannel(ChannelInfo& chInfo, MidiVoice& voice);
    int addVoiceToChannel(int midiChannel, MidiVoice& voice);

    // Returns the index the voice was in
    int removeVoiceFromChannel(ChannelInfo& chInfo, const  MidiVoice& voice);
    int removeVoiceFromChannel(int midiChannel, const MidiVoice& voice);
    int removeVoiceFromChannel(ChannelVoicePtr& chVoicePtr);

    // Returns the channel the stolen voice was assigned to
    int stealExistingVoice(int index);
    void retriggerExistingVoice(int index, int assignChannel);

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

    int getLastAssignedChannel() const { return lastChannelAssigned; }

    void setMidiNoteTuner(std::shared_ptr<MidiNoteTuner>& newTuner);

    ChannelVoicePtr findVoice(const MidiVoice& voiceToFind) const;

    const MidiVoice* findChannelAndAddVoice(NewVoiceState state, int midiChannel, int midiNote, juce::uint8 velocity);

    //const MidiVoice* getVoice(MidiVoice& voice);
    const MidiVoice* getVoice(const juce::MidiMessage& msg);
    const MidiVoice* readVoice(const juce::MidiMessage& msg) const;

    MidiVoice removeVoice(int midiChannel, int midiNote);
    MidiVoice removeVoice(const juce::MidiMessage& msg);
    //MidiVoice removeVoice(const MidiVoice* voice);

    //void clearVoices(juce::Array <MidiVoice>& voiceArray);
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