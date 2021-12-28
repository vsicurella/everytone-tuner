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


const MidiVoice* MidiVoiceController::getVoice(int index) const
{
    return voices[index];
}


const MidiVoice* MidiVoiceController::getVoice(int midiChannel, int midiNote) const
{
    auto voiceIndex = indexOfVoice(midiChannel, midiNote);
    if (voiceIndex >= 0 && voiceIndex < MULTIMAPPER_MAX_VOICES)
        return getVoice(voiceIndex);
    return nullptr;
}

const MidiVoice* MidiVoiceController::getVoice(const juce::MidiMessage& msg) const
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();
    return getVoice(channel, note);
}

int MidiVoiceController::numVoices() const
{
    //int num = 0;
    //for (int i = 0; i < voices.size(); i++)
    //{
    //    if (getVoice(i)->getAssignedChannel() >= 0)
    //        num++;
    //}

    //return num;
    return activeVoices.size();
}

juce::Array<MidiVoice> MidiVoiceController::getActiveVoices() const
{
    juce::Array<MidiVoice> activeVoicesCopy;
    for (auto voice : activeVoices)
        activeVoicesCopy.add(*voice);
    return activeVoicesCopy;
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

const MidiVoice* MidiVoiceController::addVoice(const juce::MidiMessage& msg)
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();
    auto velocity = msg.getVelocity();
    return addVoice(channel, note, velocity);
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
        auto voice = getVoice(channelIndex);
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
}

void MidiVoiceController::setChannelMode(Everytone::ChannelMode mode)
{
    channelMode = mode;
}

void MidiVoiceController::setMpeZone(Everytone::MpeZone zone)
{
    mpeZone = zone;
}

void MidiVoiceController::setVoiceLimit(int limit)
{
    voiceLimit = limit;
}

int MidiVoiceController::nextAvailableVoiceIndex() const
{
    for (int i = 0; i < voices.size(); i++)
    {
        if (channelIsFree(i))
            return i;
    }
    return -1;
}

int MidiVoiceController::nextRoundRobinVoiceIndex() const
{
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
    if (currentVoices >= voiceLimit)
        return -1;

    switch (channelMode)
    {
    case Everytone::ChannelMode::FirstAvailable:
        return nextAvailableVoiceIndex();

    case Everytone::ChannelMode::RoundRobin:
        return nextRoundRobinVoiceIndex();

    default:
        jassertfalse;
    }

    return 0;
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
        auto voice = getVoice(i);
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
        if (getVoice(i) == voice)
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

