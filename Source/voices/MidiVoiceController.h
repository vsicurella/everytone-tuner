/*
  ==============================================================================

    MidiVoiceController.h
    Created: 12 Dec 2021 2:14:00pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include "../TunerController.h"
#include "VoiceWatcher.h"
#include "VoiceBank.h"

#define EVERYTONE_LOG_VOICES 0

#define MAX_CHANNELS 16
#define MAX_VOICES_PER_CHANNEL 128

class MidiVoiceController : public VoiceChanger, private TunerController::Watcher, private juce::Timer
{
private:

    enum class NewVoiceState
    {
        Normal = 0, // Not all voices are used - get next channel
        Overflow,   // All voices are used, determine action via NotePriority
        Monophonic  // One voice at a time - continue using same channel
    };


private:

    TunerController& tuningController;

    juce::Array<std::shared_ptr<MidiNoteTuner>> usedTuners;

    Everytone::ChannelMode channelMode = Everytone::ChannelMode::FirstAvailable;
    Everytone::MpeZone mpeZone = Everytone::MpeZone::Lower;
    Everytone::NotePriority notePriority = Everytone::NotePriority::Last;

    int maxVoiceLimit = 16;
    int voiceLimit = maxVoiceLimit;

    juce::Array<bool> midiChannelDisabled;

    // Does all voice handling
    VoiceBank voiceBank;

    // For messages that need to be in the same chunk
    juce::MidiBuffer sameChunkPriorityQueue;
    int sameChunkSample = 0;

    // For messages that need to be in the next chunk
    juce::MidiBuffer nextChunkPriorityQueue;
    int nextChunkSample = 0;


    int numVoices = 0;

private:

    //int effectiveVoiceLimit() const;
    //void updateVoiceLimitCache();
    //int numVoicesAvailable() const;

    //const MidiVoice* getExistingVoicePtr(int index) const;

    void queueVoiceNoteOff(MidiVoice& voice, bool sameChunk);
    void queueVoiceNoteOn(MidiVoice& voice, bool sameChunk, bool pitchbendOnly=false);
    
    void addVoiceToChannel(int midiChannel, MidiVoice* voice);
    void removeVoiceFromChannel(int midiChannel, MidiVoice* voice);
    
    MidiVoice removeVoice(int index);

    void updateVoiceBankTuner();
    int cleanUnusedTuners();

private:

    void sourceTuningChanged(const std::shared_ptr<MappedTuningTable>& source) override;
    void targetTuningChanged(const std::shared_ptr<MappedTuningTable>& target) override;

    void timerCallback() override;

public:

    MidiVoiceController(TunerController& tuningController, 
                        Everytone::ChannelMode channelmodeIn = Everytone::ChannelMode::FirstAvailable,
                        Everytone::MpeZone mpeZone = Everytone::MpeZone::Lower);
    ~MidiVoiceController();

    Everytone::ChannelMode getChannelMode() const { return channelMode; }
    Everytone::MpeZone getMpeZone() const { return mpeZone; }
    Everytone::NotePriority getNotePriority() const { return notePriority; }
    juce::Array<bool> getChannelsDisabled() const { return midiChannelDisabled; }

    int getVoiceLimit() const { return voiceLimit; }

    // Number of all voices held, regardless if they are active
    int numAllVoices() const;

    // Array of all voices held, regardless if they are active
    juce::Array<MidiVoice> getAllVoices() const;

    // Returns a copy of the array of pointers to MidiVoices that are in the given channel
    juce::Array<MidiVoice> getVoicesInChannel(int midiChannel) const;

    int numActiveVoices() const;

    const MidiVoice* getVoice(const juce::MidiMessage& msg);
    MidiVoice removeVoice(const juce::MidiMessage& msg);

    void clearAllVoices();

    // Get messages queued for NotePriority settings, returns size of buffer
    int serveSameChunkPriorityBuffer(MidiBuffer& queueOut);
    int serveNextChunkPriorityBuffer(MidiBuffer& queueOut);

    int channelOfVoice(const juce::MidiMessage& msg) const;

    void setChannelMode(Everytone::ChannelMode mode);
    void setMpeZone(Everytone::MpeZone zone);
    void setNotePriority(Everytone::NotePriority notePriorityIn);
    void setChannelsDisabled(juce::Array<bool> channelsDisabledIn);
};
