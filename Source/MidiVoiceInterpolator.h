/*
  ==============================================================================

    MidiVoiceInterpolator.h
    Created: 27 Dec 2021 3:29:14pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "TunerController.h"
#include "MidiVoiceController.h"

class MidiVoiceInterpolator : public juce::Timer, public TunerController::Watcher
{
    Everytone::BendMode bendMode = Everytone::BendMode::Static;

    juce::Array<MidiVoice> activeVoiceTargets;

    int updateRateMs = 50;

public:

    MidiVoiceInterpolator(Everytone::BendMode bendMode = Everytone::BendMode::Static);
    ~MidiVoiceInterpolator();

    // juce::Timer implementation
    void timerCallback() override;


};