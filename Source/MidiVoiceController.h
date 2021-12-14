/*
  ==============================================================================

    MidiVoiceController.h
    Created: 12 Dec 2021 2:14:00pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include "MappedTuningController.h"
#include "MidiVoice.h"

#define MULTIMAPPER_MAX_VOICES 16

class MidiVoiceController
{
    MappedTuningController& tuningController;

    juce::OwnedArray<MidiVoice> voices;

    juce::Array<bool> midiChannelDisabled;

private:

    int nextAvailableVoiceIndex();

    int midiNoteIndex(int midiChannel, int midiNote) const;

    int indexOfVoice(int midiChannel, int midiNote) const;
    int indexOfVoice(const MidiVoice* voice) const;

    const MidiVoice* getVoice(int index) const;
    MidiVoice removeVoice(int index);

public:

    MidiVoiceController(MappedTuningController& tuningController);
    ~MidiVoiceController();

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

    void setChannelDisabled(int midiChannel, bool disabled);

};