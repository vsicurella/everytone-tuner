/*
  ==============================================================================

    MidiVoiceController.h
    Created: 12 Dec 2021 2:14:00pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include "MidiVoice.h"

#define MULTIMAPPER_MAX_VOICES 16

class MidiVoiceController
{
    juce::Array<MidiVoice> voices;

    juce::Array<bool> midiChannelDisabled;

    const Tuning* currentSourceTuning = nullptr;
    const Tuning* currentTargetTuning = nullptr;

    const Keytographer::TuningTableMap* currentMapping = nullptr;

    // Need to use reference counted stuff here
    juce::OwnedArray<MidiNoteTuner> tuners;

private:

    int nextAvailableVoiceIndex();

    int midiNoteIndex(int midiChannel, int midiNote) const;

    int indexOfVoice(int midiChannel, int midiNote) const;
    int indexOfVoice(const MidiVoice* voice) const;

    const MidiVoice* getVoice(int index);
    MidiVoice removeVoice(int index);

public:

    MidiVoiceController(const Tuning* tuningSourceIn, const Tuning* tuningTargetIn, const Keytographer::TuningTableMap* mappingIn);
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

    void setSourceTuning(const Tuning* tuning);

    void setTargetTuning(const Tuning* tuning);

    void setNoteMapping(const Keytographer::TuningTableMap* mapping);
};