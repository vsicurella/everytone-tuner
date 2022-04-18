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
      mpeZone(zoneIn),
      voiceBank(channelMode, mpeZone)
{
    
}

MidiVoiceController::~MidiVoiceController()
{

}

juce::Array<MidiVoice> MidiVoiceController::getAllVoices() const
{
    return voiceBank.getAllVoices();
}

juce::Array<MidiVoice> MidiVoiceController::getVoicesInChannel(int midiChannel) const
{
    return voiceBank.getVoicesInChannel(midiChannel);
}

int MidiVoiceController::numActiveVoices() const
{
    return voiceBank.numActiveVoices();
}

void MidiVoiceController::queueVoiceNoteOff(MidiVoice& voice, bool sameChunk)
{
    jassert(voice.isActive());
    auto noteOffMsg = voice.getNoteOff();
    if (sameChunk)
        sameChunkPriorityQueue.addEvent(noteOffMsg, sameChunkSample++);
    else
        nextChunkPriorityQueue.addEvent(noteOffMsg, nextChunkSample++);
}

void MidiVoiceController::queueVoiceNoteOn(MidiVoice& voice, bool sameChunk, bool pitchbendOnly)
{
    jassert(voice.isActive());

    auto pbmsg = voice.getPitchbend();
    if (sameChunk)
        sameChunkPriorityQueue.addEvent(pbmsg, sameChunkSample++);
    else
        nextChunkPriorityQueue.addEvent(pbmsg, nextChunkSample++);

    if (pitchbendOnly)
        return;

    auto noteOn = voice.getNoteOn();
    if (sameChunk)
        sameChunkPriorityQueue.addEvent(noteOn, sameChunkSample++);
    else
        nextChunkPriorityQueue.addEvent(noteOn, nextChunkSample++);
}
//
//void MidiVoiceController::stealExistingVoice(int index)
//{
//    if (index < 0 || index >= voices.size())
//    {
//        return;
//    }
//
//    auto voice = &voices.getReference(index);
//    if (voice->isActive())
//    {
//        auto channel = voice->getAssignedChannel();
//        removeVoiceFromChannel(channel, voice);
//
//        if (channelMode != Everytone::ChannelMode::Monophonic)
//            queueVoiceNoteOff(voice, true);
//
//        addVoiceToChannel(0, voice);
//
//        voiceWatchers.call(&MidiVoiceController::Watcher::voiceChanged, *voice);
//    }
//}
//
//void MidiVoiceController::retriggerExistingVoice(int index, int midiChannel)
//{
//    if (index >= 0)
//    {
//        auto retriggerVoice = &voices.getReference(index);
//        if (retriggerVoice->isValid())
//        {
//            removeVoiceFromChannel(0, retriggerVoice);
//            addVoiceToChannel(midiChannel, retriggerVoice);
//
//            voiceWatchers.call(&MidiVoiceController::Watcher::voiceChanged, *retriggerVoice);
//
//            // In Monophonic mode we just have to send a new pitchbend message
//            queueVoiceNoteOn(retriggerVoice, false, channelMode == Everytone::ChannelMode::Monophonic);
//        }
//    }
//}
//
//void MidiVoiceController::addVoiceToChannel(int midiChannel, MidiVoice* voice)
//{
//    jassert(midiChannel == 0 || ChannelInMidiRange(midiChannel));
//    if (midiChannel == 0 || ChannelInMidiRange(midiChannel))
//    {
//        auto channelVoices = voicesPerChannel[midiChannel];
//        channelVoices.add(voice);
//        voicesPerChannel.set(midiChannel, channelVoices);
//
//        voice->reassignChannel(midiChannel);
//
//        if (midiChannel > 0)
//            activeVoices.add(voice);
//    }
//}
//
//void MidiVoiceController::removeVoiceFromChannel(int midiChannel, MidiVoice* voice)
//{
//    if (midiChannel == 0 || ChannelInMidiRange(midiChannel))
//    {
//        auto channel = voicesPerChannel[midiChannel];
//        channel.removeFirstMatchingValue(voice);
//        voicesPerChannel.set(midiChannel, channel);
//
//        if (midiChannel > 0)
//            activeVoices.removeAllInstancesOf(voice);
//    }
//}
//
//const MidiVoice* MidiVoiceController::createAndAddVoice(int midiChannel, int midiNote, juce::uint8 velocity)
//{
//    auto newVoice = MidiVoice(midiChannel, midiNote, velocity, 0, tuningController.getTuner());
//
//    auto result = voiceBank.getVoice()
//    int newChannel = findNextVoiceChannel();
//
//    if (ChannelInMidiRange(newChannel))
//    {
//        lastChannelAssigned = newChannel;
//
//        // See if channel is occupied
//        // TODO poly channel mode
//        auto channelVoices = getVoicesInChannel(newChannel);
//        if (channelVoices.size() > 0)
//        {
//            auto voiceIndex = getNextVoiceIndexToSteal();
//            stealExistingVoice(voiceIndex);
//        }
//
//
//        voices.set(nextVoiceIndex, newVoice);
//        auto voicePointer = &voices.getReference(nextVoiceIndex);
//        addVoiceToChannel(newChannel, newVoice);
//
//        voiceWatchers.call(&MidiVoiceController::Watcher::voiceAdded, *newVoice);
//
//        return newVoice;
//    }
//     
//    return nullptr;
//}

const MidiVoice* MidiVoiceController::getVoice(const juce::MidiMessage& msg)
{
    auto voice = voiceBank.getVoice(msg);
    // todo
    return voice;
}

int MidiVoiceController::numAllVoices() const
{
    return numVoices;
}

MidiVoice MidiVoiceController::removeVoice(const juce::MidiMessage& msg)
{
    auto voice = voiceBank.removeVoice(msg);
    //todo
    return voice;
}

void MidiVoiceController::clearAllVoices()
{
    auto voices = voiceBank.getAllVoices();
    for (auto voice : voices)
    {
        queueVoiceNoteOff(voice, false);
    }

    voiceBank.clearAllVoices();
}

int MidiVoiceController::serveSameChunkPriorityBuffer(MidiBuffer& queueOut)
{
    MidiBuffer tempBuffer;

    sameChunkPriorityQueue.swapWith(tempBuffer);
    queueOut.swapWith(tempBuffer);

    int bufferSize = sameChunkSample;
    sameChunkSample = 0;
    return bufferSize;
}

int MidiVoiceController::serveNextChunkPriorityBuffer(MidiBuffer& queueOut)
{
    MidiBuffer tempBuffer;

    nextChunkPriorityQueue.swapWith(tempBuffer);
    queueOut.swapWith(tempBuffer);

    int bufferSize = nextChunkSample;
    nextChunkSample = 0;
    return bufferSize;
}

void MidiVoiceController::setChannelsDisabled(juce::Array<bool> channelsDisabled)
{
    midiChannelDisabled = channelsDisabled;
    midiChannelDisabled.resize(16);
    voiceBank.setChannelsDisabled(midiChannelDisabled);
    //updateVoiceLimitCache();
}

void MidiVoiceController::setChannelMode(Everytone::ChannelMode mode)
{
    channelMode = mode;
    voiceBank.setChannelMode(channelMode);
    juce::Logger::writeToLog("ChannelMode set to " + Everytone::getName(channelMode));
    //updateVoiceLimitCache();
}

void MidiVoiceController::setMpeZone(Everytone::MpeZone zone)
{
    mpeZone = zone;
    voiceBank.setMpeZone(mpeZone);
    juce::Logger::writeToLog("MPE Zone set to " + Everytone::getName(mpeZone));
    //updateVoiceLimitCache();
}

void MidiVoiceController::setNotePriority(Everytone::NotePriority notePriorityIn)
{
    notePriority = notePriorityIn;
    voiceBank.setNotePriority(notePriority);
    juce::Logger::writeToLog("NotePriority set to " + Everytone::getName(notePriority));
}

//int MidiVoiceController::effectiveVoiceLimit() const
//{
//    int numDisabled = 0;
//    for (auto isDisabled : midiChannelDisabled)
//        numDisabled += (isDisabled) ? 1 : 0;
//
//    int limit = maxVoiceLimit - numDisabled;
//    limit -= (mpeZone == Everytone::MpeZone::Omnichannel) ? 0 : 1;
//    return limit;
//}

//void MidiVoiceController::updateVoiceLimitCache()
//{
//    // TODO poly channel mode
//    voiceLimit = effectiveVoiceLimit();
//}

//int MidiVoiceController::numVoicesAvailable() const
//{
//    voiceBank.
//    // TODO poly channel mode
//    switch (channelMode)
//    {
//    case Everytone::ChannelMode::Monophonic:
//        return 1;
//
//    default:
//        return voiceLimit - activeVoices.size();
//    }
//}
