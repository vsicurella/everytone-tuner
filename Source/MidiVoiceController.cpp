/*
  ==============================================================================

    MidiVoiceController.cpp
    Created: 12 Dec 2021 2:14:00pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "MidiVoiceController.h"

MidiVoiceController::MidiVoiceController(TunerController& tuningControllerIn, 
                                         Everytone::ChannelMode channelModeIn,
                                         Everytone::MpeZone zoneIn,
                                         int limitIn)
    : tuningController(tuningControllerIn),
      channelMode(channelModeIn),
      mpeZone(zoneIn),
      voiceLimit(limitIn)
{
    for (int i = 0; i < MULTIMAPPER_MAX_VOICES; i++)
        voices.add(new MidiVoice());

    midiChannelDisabled.resize(16);
    midiChannelDisabled.fill(false);
}

MidiVoiceController::~MidiVoiceController()
{

}

juce::Array<MidiVoice> MidiVoiceController::getAllVoices() const
{
    juce::Array<MidiVoice> activeVoicesCopy;
    for (auto voice : activeVoices)
        activeVoicesCopy.add(*voice);
    return activeVoicesCopy;
}

int MidiVoiceController::numActiveVoices() const
{
    int num = 0;
    for (int i = 0; i < voices.size(); i++)
    {
        if (getExistingVoice(i)->isActive() >= 0)
            num++;
    }

    return num;
}

int MidiVoiceController::channelOfVoice(int midiChannel, int midiNote) const
{
    auto index = indexOfVoice(midiChannel, midiNote);
    if (index >= 0)
        return index + 1;
    return -1;
}

int MidiVoiceController::channelOfVoice(const juce::MidiMessage& msg) const
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();
    return channelOfVoice(channel, note);
}

const MidiVoice* MidiVoiceController::addVoice(int midiChannel, int midiNote, juce::uint8 velocity)
{
    auto newIndex = getNextVoiceIndex();
    if (newIndex >= 0 && newIndex < MULTIMAPPER_MAX_VOICES)
    {
        lastChannelAssigned = newIndex;
        auto newVoice = new MidiVoice(midiChannel, midiNote, velocity, newIndex + 1, tuningController.getTuner());
        voices.set(newIndex, newVoice);
        activeVoices.addIfNotAlreadyThere(newVoice);
        return getVoice(newIndex);
    }

    return nullptr;
}

MidiVoice MidiVoiceController::removeVoice(int index)
{
    if (index >= 0 && index < MULTIMAPPER_MAX_VOICES)
    {
        activeVoices.removeAllInstancesOf(voices[index]);
        auto voice = *voices[index];
        voices.set(index, new MidiVoice());
        return voice;
    }
    return MidiVoice();
}

const MidiVoice* MidiVoiceController::getExistingVoice(int index) const
{
    return voices[index];
}

const MidiVoice* MidiVoiceController::getVoice(int midiChannel, int midiNote, juce::uint8 velocity = 0)
{
    auto voiceIndex = indexOfVoice(midiChannel, midiNote);
    if (voiceIndex >= 0 && voiceIndex < MULTIMAPPER_MAX_VOICES)
        return getVoice(voiceIndex);

    return addVoice(midiChannel, midiNote, velocity);
}

const MidiVoice* MidiVoiceController::getVoice(const juce::MidiMessage& msg)
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();

    return getVoice(channel, note);
}

int MidiVoiceController::numAllVoices() const
{
    return activeVoices.size();
}

MidiVoice MidiVoiceController::removeVoice(int midiChannel, int midiNote)
{
    auto index = indexOfVoice(midiChannel, midiNote);
    return removeVoice(index);
}

MidiVoice MidiVoiceController::removeVoice(const juce::MidiMessage& msg)
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();
    return removeVoice(channel, note);
}

MidiVoice MidiVoiceController::removeVoice(const MidiVoice* voice)
{
    auto index = indexOfVoice(voice);
    return removeVoice(index);
}

bool MidiVoiceController::channelIsFree(int channelIndex, MidiPitch pitchToAssign) const
{
    bool notAvailable = midiChannelDisabled[channelIndex];

    switch (mpeZone)
    {
    case Everytone::MpeZone::Lower:
        notAvailable |= channelIndex == 0;
        break;

    case Everytone::MpeZone::Upper:
        notAvailable |= channelIndex == 15;
        break;
    }

    if (!notAvailable)
    {
        auto voice = getExistingVoice(channelIndex);
        if (voice != nullptr)
        {
            auto ch = voice->getAssignedChannel();
            if (ch < 1)
                return true;
        }
    }

    return false;
}

void MidiVoiceController::setChannelDisabled(int midiChannel, bool disabled)
{
    midiChannelDisabled.set(midiChannel - 1, disabled);
    auto message = "MIDI Channel " + juce::String((int)midiChannel);
    message += (disabled) ? " was disabled" : " was enabled";
    juce::Logger::writeToLog(message);
}

void MidiVoiceController::setChannelMode(Everytone::ChannelMode mode)
{
    channelMode = mode;
    juce::Logger::writeToLog("ChannelMode set to " + juce::String((int)channelMode));
}

void MidiVoiceController::setMpeZone(Everytone::MpeZone zone)
{
    mpeZone = zone;
    juce::Logger::writeToLog("MPE Zone set to " + juce::String((int)mpeZone));
}

void MidiVoiceController::setNotePriority(Everytone::NotePriority notePriorityIn)
{
    notePriority = notePriorityIn;
    juce::Logger::writeToLog("NotePriority set to " + juce::String((int)notePriority));
}

void MidiVoiceController::setVoiceLimit(int limit)
{
    voiceLimit = limit;
    juce::Logger::writeToLog("VoiceLimit set to " + juce::String((int)voiceLimit));
}

int MidiVoiceController::findLowestVoiceIndex(bool active) const
{
    if (currentVoices == 0)
        return 0;

    auto lowestPitch = voices[0]->getCurrentPitch();
    int lowestIndex = 0;
    for (int i = 1; i < voices.size(); i++)
    {
        auto voice = getExistingVoice(i);
        if (active && !voice->isActive())
            continue;
        
        auto voicePitch = voice->getCurrentPitch();
        if (voicePitch.comparedTo(lowestPitch) < 0)
        {
            lowestPitch = voicePitch;
            lowestIndex = i;
        }
    }

    return lowestIndex;
}

int MidiVoiceController::findHighestVoiceIndex(bool active) const
{
    if (currentVoices == 0)
        return 0;

    auto highestPitch = voices[0]->getCurrentPitch();
    int highestIndex = 0;
    for (int i = 1; i < voices.size(); i++)
    {
        auto voice = getExistingVoice(i);
        if (active && !voice->isActive())
            continue;

        auto voicePitch = voice->getCurrentPitch();
        if (voicePitch.comparedTo(highestPitch) > 0)
        {
            highestPitch = voicePitch;
            highestIndex = i;
        }
    }

    return highestIndex;
}

int MidiVoiceController::findOldestVoice(bool active) const
{
    for (int i = 0; i < activeVoices.size(); i++)
    {
        auto voice = activeVoices.getUnchecked(i);
        if (active && !voice->isActive())
            continue;

        return indexOfVoice(voice);
    }

    return 0;
}

int MidiVoiceController::findMostRecentVoice(bool active) const
{
    for (int i = 0; i < activeVoices.size(); i++)
    {
        int index = activeVoices.size() - i - 1;
        auto voice = activeVoices.getUnchecked(index);
        if (active && !voice->isActive())
            continue;

        return indexOfVoice(voice);
    }

    return 0;
}

int MidiVoiceController::nextAvailableVoiceIndex() const
{
    if (currentVoices >= voiceLimit)
    {

    }

    for (int i = 0; i < voices.size(); i++)
    {
        if (channelIsFree(i))
            return i;
    }
    return -1;
}

int MidiVoiceController::nextRoundRobinVoiceIndex() const
{
    bool stealVoice = currentVoices >= voiceLimit;

    auto i = lastChannelAssigned + 1;
    int channelsChecked = 0;
    while (channelsChecked < MULTIMAPPER_MAX_VOICES)
    {
        if (channelIsFree(i))
            return i;

        i = (i + 1) % MULTIMAPPER_MAX_VOICES;
        channelsChecked++;
    }

    return -1;
}

int MidiVoiceController::getNextVoiceIndex() const
{
    switch (channelMode)
    {
    case Everytone::ChannelMode::FirstAvailable:
        return nextAvailableVoiceIndex();

    case Everytone::ChannelMode::RoundRobin:
        return nextRoundRobinVoiceIndex();

    default:
        jassertfalse;
    }

    return -1;
}


int MidiVoiceController::midiNoteIndex(int midiChannel, int midiNote) const
{
    return (midiChannel - 1) * 128 + midiNote;
}

int MidiVoiceController::indexOfVoice(int midiChannel, int midiNote) const
{
    auto midiIndex = midiNoteIndex(midiChannel, midiNote);
    for (int i = 0; i < voices.size(); i++)
    {
        auto voice = getExistingVoice(i);
        if (voice == nullptr)
            continue;

        if (voice->getMidiNoteIndex() == midiIndex)
            return i;
    }
    return -1;
}

int MidiVoiceController::indexOfVoice(const MidiVoice* voice) const
{
    for (int i = 0; i < voices.size(); i++)
        if (getExistingVoice(i) == voice)
            return i;
    return -1;
}

int MidiVoiceController::effectiveVoiceLimit() const
{
    int numDisabled = 0;
    for (auto isDisabled : midiChannelDisabled)
        numDisabled += (isDisabled) ? 1 : 0;

    int limit = voiceLimit - numDisabled;
    limit -= (mpeZone == Everytone::MpeZone::Omnichannel) ? 0 : 1;
    return limit;
}

