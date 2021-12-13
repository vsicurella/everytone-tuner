/*
  ==============================================================================

    MidiBrain.h
    Created: 19 Nov 2021 6:23:15pm
    Author:  vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MidiVoiceController.h"

class MidiBrain
{
    MidiVoiceController& voiceController;

public:

    MidiBrain(MidiVoiceController& voiceController);

    ~MidiBrain();

    void processMidi(juce::MidiBuffer& buffer);

    int nextAvailableChannel();

    int channelOfActiveNote(const MidiPitch& pitch);

    //const Tuning* getTuningSource() const { return tuningSource.get(); }
    //const Tuning* getTuningTarget() const { return tuningTarget.get(); }

    //void setTuningSource(const Tuning& tuning);
    //void setTuningTarget(const Tuning& tuning);   

    //void setNoteMap(const Keytographer::TuningTableMap& map);
    
private:

    void preTuningChange(const Tuning& tuning);
    void postTuningChange();

    void preMapChange(const Keytographer::TuningTableMap& map);
    void postMapChange();

private:

    //juce::MidiKeyboardState state;
    
    //juce::Array<MidiPitch> channelsInUse;

    int pitchbendRange = 2;

    //std::unique_ptr<Tuning> tuningSource;
    //std::unique_ptr<Tuning> tuningTarget;

    //std::unique_ptr<Keytographer::TuningTableMap> noteMap;

    //std::unique_ptr<MidiNoteTuner> tuner;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiBrain)
};
