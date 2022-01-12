/*
  ==============================================================================

    MidiVoiceInterpolator.cpp
    Created: 27 Dec 2021 3:29:14pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "MidiVoiceInterpolator.h"

MidiVoiceInterpolator::MidiVoiceInterpolator(MidiVoiceController& voiceControllerIn, Everytone::BendMode bendModeIn)
    : voiceController(voiceControllerIn)
{
    setBendMode(bendModeIn);
}

MidiVoiceInterpolator::~MidiVoiceInterpolator()
{

}


// juce::Timer implementation
void MidiVoiceInterpolator::timerCallback()
{
    updateTargetVoices();
}


juce::Array<MidiVoice>MidiVoiceInterpolator:: getVoiceTargets() const
{
    return activeVoiceTargets;
}

void MidiVoiceInterpolator::clearVoiceTargets()
{
    activeVoiceTargets.clearQuick();
}

juce::Array<MidiVoice> MidiVoiceInterpolator::getAndClearVoiceTargets()
{
    auto voices = activeVoiceTargets;
    activeVoiceTargets.clearQuick();
    return voices;
}

void MidiVoiceInterpolator::setBendMode(Everytone::BendMode bendModeIn)
{
    bendMode = bendModeIn;
    juce::Logger::writeToLog("Set BendMode to " + juce::String((int)bendModeIn));
    switch (bendMode)
    {
    case Everytone::BendMode::Dynamic:
    case Everytone::BendMode::Persistent:
        startTimer(updateRateMs);
        return;

    default:
        jassertfalse;
    case Everytone::BendMode::Static:
        stopTimer();
        clearVoiceTargets();
        return;
    }
}

void MidiVoiceInterpolator::updateTargetVoices()
{
    switch (bendMode)
    {
    case Everytone::BendMode::Persistent:
        activeVoiceTargets = voiceController.getAllVoices();
        break;

    case Everytone::BendMode::Dynamic:
        // TODO
        jassertfalse;
        break;
    }
}