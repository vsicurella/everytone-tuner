/*
  ==============================================================================

    MidiVoice.h
    Created: 12 Dec 2021 2:15:00pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "MidiNoteTuner.h"

struct LinkedController
{
    int number = -1;
    juce::uint8 value;
};

class MidiVoice
{
    const int midiChannel = -1;
    const int midiNote = -1;

    juce::uint8 velocity = 0;
    juce::uint8 aftertouch = 0;

    const int assignedChannel = -1;

    juce::Array<LinkedController> controllers;

    std::shared_ptr<MidiNoteTuner> tuner;

    //MappedNote currentMappedNote;
    int currentTuningIndex;
    MidiPitch currentPitch;

private:

    int findControllerIndex(int number);

public:

    MidiVoice() {}
  
    MidiVoice(int midiChannel, int midiNote, juce::uint8 velocity, int assignedChannel, std::shared_ptr<MidiNoteTuner> tuner);
    
    ~MidiVoice() {}

    int getMidiChannel() const { return midiChannel; }

    int getMidiNote() const { return midiNote; }

    int getMidiNoteIndex() const { return (midiChannel - 1) * 128 + midiNote; }

    int getAssignedChannel() const { return assignedChannel; }
    
    void updateMapping();

    void updatePitch();

    void update();

    void updateAftertouch(juce::uint8 aftertouch);

    void updateController(int number, juce::uint8 value);

    juce::uint8 getControllerValue(int number);

    juce::MidiMessage getNoteOn() const;

    juce::MidiMessage getPitchbend() const;

    juce::MidiMessage getAftertouch(int value) const;

    juce::MidiMessage getNoteOff() const;

    void mapMidiMessage(juce::MidiMessage& msg) const;
};