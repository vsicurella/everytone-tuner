/*
  ==============================================================================

    MidiBrain.h
    Created: 19 Nov 2021 6:23:15pm
    Author:  vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MidiNoteTuner.h"

class MidiBrain
{
public:

    MidiBrain();

    ~MidiBrain();

    void processMidi(juce::MidiBuffer& buffer);

    int nextAvailableChannel();

    int channelOfActiveNote(int noteNumber);

private:

    juce::MidiKeyboardState midiInputState;
    
    juce::Array<int> channelsInUse;

    int pitchbendRange = 2;

    std::unique_ptr<Tuning> oldTuning;
    std::unique_ptr<Tuning> newTuning;

    std::unique_ptr<MidiNoteTuner> tuner;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiBrain)
};
