/*
  ==============================================================================

    MidiVoice.h
    Created: 12 Dec 2021 2:15:00pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "../MidiNoteTuner.h"

//struct LinkedController
//{
//    int number = -1;
//    juce::uint8 value;
//};

class MidiVoice
{
    int midiChannel = -1;
    int midiNote = -1;

    juce::uint8 velocity = 0;
    juce::uint8 aftertouch = 0;

    // below 0 and above 16 is reserved for invalid voices
    // 0 is reserved for inactive voices
    // 1 through 16 are active voices
    int assignedChannel = -1;

    //juce::Array<LinkedController> controllers;

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

    void operator=(const MidiVoice& voice);

    bool operator==(const MidiVoice& voice) const
    {
        return tuner.get() == voice.tuner.get()
            && midiChannel == voice.midiChannel
            && midiNote == voice.midiNote;
        // match assigned channel too?
    }

    bool operator!=(const MidiVoice& voice) const { return !operator==(voice); }

    int getMidiChannel() const { return midiChannel; }

    int getMidiNote() const { return midiNote; }

    int getMidiNoteIndex() const { return (midiChannel - 1) * 128 + midiNote; }

    int getAssignedChannel() const { return assignedChannel; }

    MidiPitch getCurrentPitch() const { return currentPitch; }

    bool isValid() const { return assignedChannel >= 0 && assignedChannel <= 16; }

    bool isInvalid() const { return assignedChannel < 0 || assignedChannel > 16; }

    bool isActive() const { return assignedChannel > 0 && assignedChannel <= 16; }
    
    void reassignChannel(int midiChannel) { assignedChannel = midiChannel; }

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