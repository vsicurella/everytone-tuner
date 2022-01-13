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
                                         Everytone::MpeZone zoneIn)
    : tuningController(tuningControllerIn),
      channelMode(channelModeIn),
      mpeZone(zoneIn)
{
    for (int i = 0; i < maxVoiceLimit; i++)
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
    for (auto voice : heldVoices)
        activeVoicesCopy.add(*voice);
    return activeVoicesCopy;
}

int MidiVoiceController::numHeldVoices() const
{
    return heldVoices.size();
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

const MidiVoice* MidiVoiceController::getExistingVoice(int index) const
{
    return voices[index];
}

void MidiVoiceController::stealExistingVoice(int index)
{
    auto voice = voices[index];
    if (voice->isValid())
    {
        auto noteOffMsg = voice->getNoteOff();
        notePriorityQueue.addEvent(noteOffMsg, notePrioritySample++);
        voice->reassignChannel(0);
    }
}

MidiVoice* MidiVoiceController::createVoice(int index, int midiChannel, int midiNote, juce::uint8 velocity)
{
    auto newVoice = new MidiVoice(midiChannel, midiNote, velocity, index + 1, tuningController.getTuner());
    
    auto existingVoice = getExistingVoice(index);
    
    // New voice in empty spot
    if (existingVoice->isInvalid())
        return newVoice;

    // Set existing voice as inactive and take channel
    if (existingVoice->isActive())
    {
        stealExistingVoice(index);
        return newVoice;
    }

    // For now we shouldn't find an inactive valid voice
    jassertfalse;
    return nullptr;
}

const MidiVoice* MidiVoiceController::findChannelAndAddVoice(int midiChannel, int midiNote, juce::uint8 velocity)
{
    auto newIndex = getNextVoiceIndex();
    if (newIndex >= 0 && newIndex < maxVoiceLimit)
    {
        auto newVoice = createVoice(newIndex, midiChannel, midiNote, velocity);
        if (newVoice == nullptr)
            return nullptr;

        lastChannelAssigned = newIndex;
        voices.set(newIndex, newVoice);
        heldVoices.addIfNotAlreadyThere(newVoice);
        return getVoice(newIndex);
    }
     
    return nullptr;
}

void MidiVoiceController::retriggerExistingVoice(int index, int midiChannel)
{
    if (index >= 0)
    {
        auto retriggerVoice = voices[index];
        if (retriggerVoice->isValid())
        {
            retriggerVoice->reassignChannel(midiChannel);
            auto pbmsg = retriggerVoice->getPitchbend();
            auto noteOn = retriggerVoice->getNoteOn();
            notePriorityQueue.addEvent(pbmsg, notePrioritySample++);
            notePriorityQueue.addEvent(noteOn, notePrioritySample++);
        }
    }
}

MidiVoice MidiVoiceController::removeVoice(int index)
{
    if (index >= 0 && index < maxVoiceLimit)
    {
        heldVoices.removeFirstMatchingValue(voices[index]);
        auto voice = *voices[index];

        auto retriggerIndex = getNextVoiceToRetrigger();
        retriggerExistingVoice(retriggerIndex, voice.getAssignedChannel());

        voices.set(index, new MidiVoice());
        return voice;
    }
    return MidiVoice();
}

const MidiVoice* MidiVoiceController::getVoice(int midiChannel, int midiNote, juce::uint8 velocity = 0)
{
    auto voiceIndex = indexOfVoice(midiChannel, midiNote);
    if (voiceIndex >= 0 && voiceIndex < maxVoiceLimit)
        return getVoice(voiceIndex);

    return findChannelAndAddVoice(midiChannel, midiNote, velocity);
}

const MidiVoice* MidiVoiceController::getVoice(const juce::MidiMessage& msg)
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();
    return getVoice(channel, note);
}

int MidiVoiceController::numAllVoices() const
{
    return heldVoices.size();
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

MidiBuffer MidiVoiceController::serveNotePriorityMessages()
{
    MidiBuffer queue;
    notePriorityQueue.swapWith(queue);
    notePrioritySample = 0;
    return queue;
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
    if (numHeldVoices() == 0)
        return 0;

    auto lowestPitch = heldVoices[0]->getCurrentPitch();
    int lowestIndex = 0;
    for (int i = 1; i < heldVoices.size(); i++)
    {
        auto voice = heldVoices[i];
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
    if (numHeldVoices() == 0)
        return 0;

    auto highestPitch = heldVoices[0]->getCurrentPitch();
    int highestIndex = 0;
    for (int i = 1; i < heldVoices.size(); i++)
    {
        auto voice = heldVoices[i];
        if (active && !voice->isActive())
            continue;

        if (!active && voice->isActive())
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
    for (int i = 0; i < heldVoices.size(); i++)
    {
        auto voice = heldVoices.getUnchecked(i);
        if (active && !voice->isActive())
            continue;

        return indexOfVoice(voice);
    }

    return 0;
}

int MidiVoiceController::findMostRecentVoice(bool active) const
{
    for (int i = 0; i < heldVoices.size(); i++)
    {
        int index = heldVoices.size() - i - 1;
        auto voice = heldVoices.getUnchecked(index);
        if (active && !voice->isActive())
            continue;

        return indexOfVoice(voice);
    }

    return 0;
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
    while (channelsChecked < maxVoiceLimit)
    {
        if (channelIsFree(i))
            return i;

        i = (i + 1) % maxVoiceLimit;
        channelsChecked++;
    }

    return -1;
}

int MidiVoiceController::getNextVoiceIndexToSteal() const
{
    switch (notePriority)
    {
    case Everytone::NotePriority::Highest:
        return findLowestVoiceIndex(true);

    case Everytone::NotePriority::Lowest:
        return findHighestVoiceIndex(true);

    case Everytone::NotePriority::Last:
        return findOldestVoice(true);
    
    default:
        jassertfalse;
    }

    return -1;
}

int MidiVoiceController::getNextVoiceToRetrigger() const
{
    switch (notePriority)
    {
    case Everytone::NotePriority::Highest:
        return findHighestVoiceIndex(false);

    case Everytone::NotePriority::Lowest:
        return findLowestVoiceIndex(false);

    case Everytone::NotePriority::Last:
        return findMostRecentVoice(false);

    default:
        jassertfalse;
    }

    return -1;
}

int MidiVoiceController::getNextVoiceIndex() const
{
    if (numHeldVoices() >= voiceLimit)
    {
        return getNextVoiceIndexToSteal();
    }

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

