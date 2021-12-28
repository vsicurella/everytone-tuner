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

class MidiVoiceInterpolator : public juce::Timer
{
    const MidiVoiceController& voiceController;

    Everytone::BendMode bendMode = Everytone::BendMode::Static;

    juce::Array<MidiVoice> activeVoiceTargets;

    int updateRateMs = 50;

public:

    MidiVoiceInterpolator(MidiVoiceController& voiceController, Everytone::BendMode bendMode = Everytone::BendMode::Static);
    ~MidiVoiceInterpolator();

    Everytone::BendMode getBendMode() const { return bendMode; }

    // juce::Timer implementation
    void timerCallback() override;

    juce::Array<MidiVoice> getVoiceTargets() const;

    void clearVoiceTargets();

    juce::Array<MidiVoice> getAndClearVoiceTargets();

    void setBendMode(Everytone::BendMode bendModeIn);

private:

    void updateTargetVoices();
};