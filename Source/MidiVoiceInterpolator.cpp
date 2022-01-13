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

void MidiVoiceInterpolator::voiceAdded(MidiVoice voice)
{
    voices.add(voice);
    targetsNeedUpdate = true;
}

void MidiVoiceInterpolator::voiceChanged(MidiVoice voice)
{
    bool found = false;
    for (int i = 0; i < activeVoiceTargets.size(); i++)
    {
        auto activeVoice = activeVoiceTargets[i];
        if (activeVoice.getMidiNoteIndex() == voice.getMidiNoteIndex())
        {
            if (!voice.isActive())
            {
                found = true;
                voices.remove(i);
                break;
            }
        }
    }

    if (!found)
    {
        voices.add(voice);
        targetsNeedUpdate = true;
    }

}

void MidiVoiceInterpolator::voiceRemoved(MidiVoice voice)
{
    voices.removeAllInstancesOf(voice);
    targetsNeedUpdate = true;
}

juce::Array<MidiVoice>MidiVoiceInterpolator:: getVoiceTargets() const
{
    return activeVoiceTargets;
}

void MidiVoiceInterpolator::clearVoiceTargets()
{
    activeVoiceTargets.clearQuick();
    targetsNeedUpdate = false;
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
    if (!targetsNeedUpdate)
        return;

    switch (bendMode)
    {
    case Everytone::BendMode::Persistent:
        activeVoiceTargets = voices;
        break;

    case Everytone::BendMode::Dynamic:
        // TODO
        jassertfalse;
        break;
    }
}