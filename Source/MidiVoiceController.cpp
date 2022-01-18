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
    resetVoicesPerChannel();
    midiChannelDisabled.resize(16);
    midiChannelDisabled.fill(false);
}

MidiVoiceController::~MidiVoiceController()
{
    voices.clear();
}

void MidiVoiceController::resetVoicesPerChannel()
{
    for (int i = 0; i <= 16; i++)
        voicesPerChannel.add(juce::Array<MidiVoice*>());
}

MidiVoiceController::NewVoiceState MidiVoiceController::getNewVoiceState() const
{
    if (channelMode == Everytone::ChannelMode::Monophonic)
        return MidiVoiceController::NewVoiceState::Monophonic;
        
    if (numActiveVoices() <= voiceLimit)
        return MidiVoiceController::NewVoiceState::Overflow;

    return MidiVoiceController::NewVoiceState::Normal;
}

juce::Array<MidiVoice> MidiVoiceController::getAllVoices() const
{
    juce::Array<MidiVoice> activeVoicesCopy;
    for (auto voice : voices)
        activeVoicesCopy.add(*voice);
    return activeVoicesCopy;
}

juce::Array<MidiVoice*> MidiVoiceController::getVoicesInChannel(int midiChannel) const
{
    if (midiChannel >= 0 && midiChannel < voicesPerChannel.size())
        return voicesPerChannel[midiChannel];
    
    return juce::Array<MidiVoice*>();
}

int MidiVoiceController::numActiveVoices() const
{
    return activeVoices.size();
}

int MidiVoiceController::channelOfVoice(MidiVoice* voice) const
{
    return channelOfVoice(voice->getMidiChannel(), voice->getMidiNote());
}

int MidiVoiceController::channelOfVoice(int midiChannel, int midiNote) const
{
    for (int ch = 0; ch < voicesPerChannel.size(); ch++)
    {
        auto channel = voicesPerChannel.getReference(ch);
        for (auto voice : channel)
            if (voice->getMidiChannel() == midiChannel && voice->getMidiNote() == midiNote)
                return ch;
    }

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
    if (index >= 0 && index < voices.size())
        return voices[index];
    return nullptr;
}

void MidiVoiceController::queueVoiceNoteOff(MidiVoice* voice)
{
    jassert(voice->isActive());
    auto noteOffMsg = voice->getNoteOff();
    notePriorityQueue.addEvent(noteOffMsg, notePrioritySample++);
}

void MidiVoiceController::queueVoiceNoteOn(MidiVoice* voice)
{
    jassert(voice->isActive());
    auto pbmsg = voice->getPitchbend();
    auto noteOn = voice->getNoteOn();
    notePriorityQueue.addEvent(pbmsg, notePrioritySample++);
    notePriorityQueue.addEvent(noteOn, notePrioritySample++);
}

void MidiVoiceController::stealExistingVoice(int index)
{
    auto voice = voices[index];
    if (voice->isActive())
    {
        auto channel = voice->getAssignedChannel();
        removeVoiceFromChannel(channel, voice);

        queueVoiceNoteOff(voice);

        addVoiceToChannel(0, voice);

        voiceWatchers.call(&MidiVoiceController::Watcher::voiceChanged, *voice);
    }
}

void MidiVoiceController::retriggerExistingVoice(int index, int midiChannel)
{
    if (index >= 0)
    {
        auto retriggerVoice = voices[index];
        if (retriggerVoice->isValid())
        {
            retriggerVoice->reassignChannel(midiChannel);

            removeVoiceFromChannel(0, retriggerVoice);
            addVoiceToChannel(midiChannel, retriggerVoice);

            voiceWatchers.call(&MidiVoiceController::Watcher::voiceChanged, *retriggerVoice);

            queueVoiceNoteOn(retriggerVoice);
        }
    }
}

void MidiVoiceController::addVoiceToChannel(int midiChannel, MidiVoice* voice)
{
    jassert(midiChannel == 0 || ChannelInMidiRange(midiChannel));
    if (midiChannel == 0 || ChannelInMidiRange(midiChannel))
    {
        voicesPerChannel.getReference(midiChannel).add(voice);
        voice->reassignChannel(midiChannel);

        if (midiChannel > 0)
            activeVoices.add(voice);
    }
}

void MidiVoiceController::removeVoiceFromChannel(int midiChannel, MidiVoice* voice)
{
    if (midiChannel == 0 || ChannelInMidiRange(midiChannel))
    {
        auto channel = voicesPerChannel[midiChannel];
        channel.removeFirstMatchingValue(voice);

        voicesPerChannel.set(midiChannel, channel);

        if (midiChannel > 0)
            activeVoices.removeAllInstancesOf(voice);
    }
}

const MidiVoice* MidiVoiceController::findChannelAndAddVoice(int midiChannel, int midiNote, juce::uint8 velocity)
{
    auto newVoice = new MidiVoice(midiChannel, midiNote, velocity, 0, tuningController.getTuner());

    int newChannel = findNextVoiceChannel();

    if (ChannelInMidiRange(newChannel))
    {
        if (newVoice == nullptr)
            return nullptr;

        lastChannelAssigned = newChannel;

        // See if channel is occupied
        // TODO poly channel mode
        auto channelVoices = getVoicesInChannel(newChannel);
        if (channelVoices.size() > 0)
        {
            auto voiceIndex = indexOfVoice(channelVoices[0]);
            stealExistingVoice(voiceIndex);
        }

        voices.add(newVoice);
        addVoiceToChannel(newChannel, newVoice);

        voiceWatchers.call(&MidiVoiceController::Watcher::voiceAdded, *newVoice);

        return newVoice;
    }
     
    return nullptr;
}

MidiVoice MidiVoiceController::removeVoice(int index)
{
    jassert(index >= 0 && index < maxVoiceLimit);
    if (index >= 0 && index < maxVoiceLimit)
    {
        auto voice = voices[index];
        removeVoiceFromChannel(voice->getAssignedChannel(), voice);

        auto retriggerIndex = getNextVoiceToRetrigger();
        retriggerExistingVoice(retriggerIndex, voice->getAssignedChannel());

        auto voiceCopy = *voice;
        voices.remove(index);

        voiceWatchers.call(&MidiVoiceController::Watcher::voiceRemoved, voiceCopy);

        return voiceCopy;
    }
    return MidiVoice();
}

const MidiVoice* MidiVoiceController::getVoice(int midiChannel, int midiNote, juce::uint8 velocity)
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
    return voices.size();
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

void MidiVoiceController::clearAllVoices()
{
    for (auto voice : voices)
    {
        queueVoiceNoteOff(voice);
        removeVoiceFromChannel(voice->getAssignedChannel(), voice);
    }

    activeVoices.clear();
    resetVoicesPerChannel();
    voices.clear();
}

int MidiVoiceController::serveNotePriorityMessages(MidiBuffer& queueOut)
{
    MidiBuffer tempBuffer;
    notePriorityQueue.swapWith(tempBuffer);

    int bufferSize = notePrioritySample;
    queueOut.swapWith(tempBuffer);

    notePrioritySample = 0;
    return bufferSize;
}

bool MidiVoiceController::channelIsFree(int midiChannel, MidiPitch pitchToAssign) const
{
    if (!ChannelInMidiRange(midiChannel))
    {
        jassertfalse;
        return false;
    }

    bool notAvailable = midiChannelDisabled[midiChannel - 1];

    switch (mpeZone)
    {
    case Everytone::MpeZone::Lower:
        notAvailable |= midiChannel == 1;
        break;

    case Everytone::MpeZone::Upper:
        notAvailable |= midiChannel == 16;
        break;
    }

    if (!notAvailable)
    {
        auto channelVoices = voicesPerChannel.getReference(midiChannel);

        // TODO Poly channel mode
        if (channelVoices.size() == 0)
            return true;
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
    if (numActiveVoices() == 0)
        return 0;

    auto lowestPitch = voices[0]->getCurrentPitch();
    int lowestIndex = 0;
    for (int i = 1; i < voices.size(); i++)
    {
        auto voice = voices[i];
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
    if (numActiveVoices() == 0)
        return 0;

    auto highestPitch = voices[0]->getCurrentPitch();
    int highestIndex = 0;
    for (int i = 1; i < voices.size(); i++)
    {
        auto voice = voices[i];
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

int MidiVoiceController::findOldestVoiceIndex(bool active) const
{
    for (int i = 0; i < voices.size(); i++)
    {
        auto voice = voices.getUnchecked(i);
        if (active && !voice->isActive())
            continue;

        return indexOfVoice(voice);
    }

    return 0;
}

int MidiVoiceController::findMostRecentVoiceIndex(bool active) const
{
    for (int i = 0; i < voices.size(); i++)
    {
        int index = voices.size() - i - 1;
        auto voice = voices.getUnchecked(index);
        if (active && !voice->isActive())
            continue;

        return indexOfVoice(voice);
    }

    return 0;
}

int MidiVoiceController::nextAvailableChannel() const
{
    // TODO Poly channel mode

    for (int ch = 1; ch < voicesPerChannel.size(); ch++)
    {
        if (channelIsFree(ch))
            return ch;
    }
    return -1;
}

int MidiVoiceController::nextRoundRobinChannel() const
{
    // TODO Poly channel mode

    auto i = (lastChannelAssigned + 1) % 16;

    int channelsChecked = 1;
    while (channelsChecked < voicesPerChannel.size())
    {
        auto ch = i + 1;
        if (channelIsFree(ch))
            return ch;

        i = ch % 16;
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
        return findOldestVoiceIndex(true);
    
    default:
        jassertfalse;
    }

    return -1;
}

int MidiVoiceController::getNextVoiceToRetrigger() const
{
    auto inactiveVoices = getVoicesInChannel(0);
    if (inactiveVoices.size() == 0)
        return -1;

    switch (notePriority)
    {
    case Everytone::NotePriority::Highest:
        return findHighestVoiceIndex(false);

    case Everytone::NotePriority::Lowest:
        return findLowestVoiceIndex(false);

    case Everytone::NotePriority::Last:
        return findMostRecentVoiceIndex(false);

    default:
        jassertfalse;
    }

    return -1;
}

int MidiVoiceController::findNextVoiceChannel(MidiPitch pitchOfNewVoice) const
{
    auto newVoiceState = getNewVoiceState();

    int newChannel = -1;

    switch (newVoiceState)
    {
    case MidiVoiceController::NewVoiceState::Monophonic:
        //todo
        break;

    case MidiVoiceController::NewVoiceState::Overflow:
    {
        auto stealIndex = getNextVoiceIndexToSteal();
        auto voiceToSteal = getExistingVoice(stealIndex);
        if (voiceToSteal != nullptr)
        {
            newChannel = voiceToSteal->getAssignedChannel();
            //stealExistingVoice(stealIndex);
        }
    }

    case MidiVoiceController::NewVoiceState::Normal:
    {
        switch (channelMode)
        {
        case Everytone::ChannelMode::FirstAvailable:
            newChannel = nextAvailableChannel();
            break;

        case Everytone::ChannelMode::RoundRobin:
            newChannel = nextRoundRobinChannel();
            break;

        default:
            jassertfalse;
        }

        break;
    }

    default:
        jassertfalse;
    }

    return newChannel;
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

