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
    TunerController& tuningController;

    juce::OwnedArray<MidiVoice> voices;

    juce::Array<bool> midiChannelDisabled;

    Everytone::ChannelMode channelMode = Everytone::ChannelMode::FirstAvailable;
    Everytone::MpeZone mpeZone = Everytone::MpeZone::Lower;

    int voiceLimit = MULTIMAPPER_MAX_VOICES;

    int currentVoices = 0;
    int lastChannelAssigned = 0;

private:

    int nextAvailableVoiceIndex() const;
    int nextRoundRobinVoiceIndex() const;
    int getNextVoiceIndex() const;

    int midiNoteIndex(int midiChannel, int midiNote) const;

    int indexOfVoice(int midiChannel, int midiNote) const;
    int indexOfVoice(const MidiVoice* voice) const;

    int effectiveVoiceLimit() const;

    const MidiVoice* getVoice(int index) const;
    MidiVoice removeVoice(int index);

public:

    MidiVoiceController(TunerController& tuningController);
    ~MidiVoiceController();

    Everytone::ChannelMode getChannelMode() const { return channelMode; }
    Everytone::MpeZone getMpeZone() const { return mpeZone; }

    int getVoiceLimit() const { return voiceLimit; }


    const MidiVoice* getVoice(int midiChannel, int midiNote) const;
    const MidiVoice* getVoice(const juce::MidiMessage& msg) const;

    //const MidiVoice* getVoiceWithPitch(MidiPitch pitch) const;

    int numVoices() const;

    int channelOfVoice(int midiChannel, int midiNote) const;
    int channelOfVoice(const juce::MidiMessage& msg) const;


    const MidiVoice* addVoice(int midiChannel, int midiNote, juce::uint8 velocity);
    const MidiVoice* addVoice(const juce::MidiMessage& msg);

    MidiVoice removeVoice(int midiChannel, int midiNote);
    MidiVoice removeVoice(const juce::MidiMessage& msg);
    MidiVoice removeVoice(const MidiVoice* voice);

    bool channelIsFree(int channelNumber, MidiPitch pitchToAssign = MidiPitch()) const;

    void setChannelDisabled(int midiChannel, bool disabled);
    
    void setChannelMode(Everytone::ChannelMode mode);
    void setMpeZone(Everytone::MpeZone zone);
    void setVoiceLimit(int voiceLimit);

};