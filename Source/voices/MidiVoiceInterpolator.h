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

class MidiVoiceInterpolator : public juce::Timer, public MidiVoiceController::Watcher
{
    const MidiVoiceController& voiceController;

    Everytone::BendMode bendMode = Everytone::BendMode::Static;

    // Voices that are held, changed by MidiVoiceController::Watcher implementation
    juce::Array<MidiVoice> voices;

    // Voices to send update messages for
    juce::Array<MidiVoice> activeVoiceTargets;

    bool targetsNeedUpdate = false;

    int updateRateMs = 50;

public:

    MidiVoiceInterpolator(MidiVoiceController& voiceController, Everytone::BendMode bendMode = Everytone::BendMode::Static);
    ~MidiVoiceInterpolator();

    Everytone::BendMode getBendMode() const { return bendMode; }

    // juce::Timer implementation
    void timerCallback() override;

    // MidiVoiceController::Watcher implementation
    void voiceAdded(MidiVoice voice) override;
    void voiceChanged(MidiVoice voice) override;
    void voiceRemoved(MidiVoice voice) override;

    juce::Array<MidiVoice> getVoiceTargets() const;

    void clearVoiceTargets();

    juce::Array<MidiVoice> getAndClearVoiceTargets();

    void setBendMode(Everytone::BendMode bendModeIn);

private:

    void updateTargetVoices();
};