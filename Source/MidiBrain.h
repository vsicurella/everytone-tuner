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

    void setTuningSource(const Tuning& tuning);
    void setTuningTarget(const Tuning& tuning);

    void setNoteMap(const Keytographer::TuningTableMap& map);
    
private:

    void preTuningChange(const Tuning& tuning);
    void postTuningChange();

    void preMapChange(const Keytographer::TuningTableMap& map);
    void postMapChange();

private:

    juce::MidiKeyboardState state;
    juce::MidiKeyboardState midiInputState;
    
    juce::Array<int> channelsInUse;

    int pitchbendRange = 2;

    std::unique_ptr<Tuning> tuningSource;
    std::unique_ptr<Tuning> tuningTarget;

    std::unique_ptr<Keytographer::TuningTableMap> noteMap;

    std::unique_ptr<MidiNoteTuner> tuner;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiBrain)
};
