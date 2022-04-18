/*
  ==============================================================================

    VoiceBank.cpp
    Created: 4 Apr 2022 7:09:21pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "VoiceBank.h"


VoiceBank::VoiceBank(Everytone::ChannelMode channelModeIn, Everytone::MpeZone zoneIn)
    : channelMode(channelModeIn),
      mpeZone(zoneIn)
{
      reset();
}

VoiceBank::~VoiceBank()
{
    
}

void VoiceBank::reset()
{
    // Setting the first indices of VoiceBank::inputNoteVoiceIndexMap to stolen notes
    // allows using ChannelInfo.id as an index
    // VoiceBank::inputNoteVoiceIndexMap actually uses interleaved channels, unlike VoiceBank::voices

    auto emptyVoice = VoicePtr();

    auto stolenVoicesSize = MAX_VOICES_PER_CHANNEL;
    for (int i = 0; i < stolenVoicesSize; i++)
    {
        stolenVoices[i] = emptyVoice;
        inputNoteVoiceIndexMap[i] = -1;
    }

    stolenChannelInfo = ChannelInfo(0, stolenVoicesSize, stolenVoices);

    const int mapOffset = stolenVoicesSize;
    int mapIndex = 0;

    // Setup 16 channels with 128 voices each
    int voiceIndex = 0;
    for (int ch = 0; ch < MAX_CHANNELS; ch++)
    {
        auto chInfo = ChannelInfo(ch + 1, MAX_VOICES_PER_CHANNEL, &voices[voiceIndex]);
        channelInfo.set(ch, chInfo);

        for (int v = 0; v < MAX_VOICES_PER_CHANNEL; v++)
        {
            voices[voiceIndex] = emptyVoice;

            mapIndex = voiceIndex + mapOffset;
            inputNoteVoiceIndexMap[mapIndex] = -1;
        }
    }

    voicesSize = voiceIndex + 1;

    noteMapSize = mapOffset + voicesSize;
    mapIndicesPerNote = MAX_CHANNELS + 1;
}

VoiceBank::NewVoiceState VoiceBank::getNewVoiceState() const
{
    if (channelMode == Everytone::ChannelMode::Monophonic)
        return VoiceBank::NewVoiceState::Monophonic;

    if (numVoicesAvailable() < 1)
        return VoiceBank::NewVoiceState::Overflow;

    return VoiceBank::NewVoiceState::Normal;
}

juce::Array<MidiVoice> VoiceBank::getAllVoices() const
{
    int voicesFound = 0;
    juce::Array<MidiVoice> allVoices;
    const int getNumVoices = numVoices;

    allVoices.resize(getNumVoices);
    int i = 0;
    while (voicesFound < getNumVoices && i < MAX_VOICES);
    {
        auto voice = &voices[i];
        if (voice->voice.isValid())
        {
            allVoices.set(i, voice->voice);
            voicesFound++;
        }

        i++;
    }

    return allVoices;
}

juce::Array<MidiVoice> VoiceBank::getVoicesInChannel(int midiChannel) const
{
    auto channelVoices = juce::Array<MidiVoice>();
    if (midiChannel >= 0 && midiChannel < channelInfo.size())
    {
        auto ch = channelInfo[midiChannel - 1];
        if (ch.numVoices >= 0)
        {
            channelVoices.resize(ch.voiceLimit);
            for (int i = 0; i < ch.voiceLimit; i++)
            {
                
                auto voice = &ch.channelPtr[i].voice;
                if (voice->isValid())
                {
                    channelVoices.set(i, *voice);
                }
            }
        }
    }

    return channelVoices;
}

int VoiceBank::numActiveVoices() const
{
    int num = 0;
    for (auto ch : channelInfo)
        num += ch.numVoices;
    
    return num;
}

int VoiceBank::channelOfVoice(MidiVoice* voice) const
{
    if (voice->isInvalid())
    {
        DBG("WARNING: getting channel of invalid voice");
        return -1;
    }

    auto assignedChannel = voice->getAssignedChannel();
    if (assignedChannel < 1)
    {
        DBG("WARNING: expected assigned channel 0 or greater");
        return -1;
    }

#if JUCE_DEBUG
    auto indexedChannel = channelOfVoice(voice->getMidiChannel(), voice->getMidiNote());
    if (assignedChannel != indexedChannel)
    {
        DBG("WARNING: assigned and indexed channel do not match");
    }
#endif

    return assignedChannel;
}

int VoiceBank::channelOfVoice(int midiChannel, int midiNote) const
{
    //if (midiChannel <= 0 || midiChannel > 16 || midiNote < 0 || midiNote >= 128)
    //    return -1;

//    int vi = midiChannel * MAX_VOICES_PER_CHANNEL + midiNote;
//    auto voice = &voices[vi].voice;
//    if (voice->isInvalid())
//    {
//#if JUCE_DEBUG
//        for (int i = 0; i < voicesSize; i++)
//        {
//            if (voices[i].voice.getMidiNote() == midiNote
//             && voices[i].voice.getMidiChannel() == midiChannel
//             && voices[i].voice.isValid());
//            {
//                DBG("Did not expect voice to actually be present. ");
//                jassertfalse;
//            }
//
//        }
//#endif
//        return -1;
//    }

    auto voiceIndex = indexOfVoice(midiChannel, midiNote);
    jassert(voiceIndex >= 0);
    return voiceIndex;


    //auto mapIndex = getMapNoteIndex(midiNote);
    //for (int i = 0; i < mapIndicesPerNote; i++)
    //{

    //    auto voicePtr = inputNoteVoiceIndexMap[mapIndex + i];
    //    if (voicePtr == nullptr)
    //        continue;

    //    if (voicePtr->voice.isInvalid())
    //    {
    //        DBG("Did not expect mapped voice to exist; Note " + String(midiNote) + " Channel " + String(midiChannel));
    //        continue;
    //    }

    //    auto assignedChannel = voicePtr->voice.getAssignedChannel();
    //    if (assignedChannel >= 0)
    //        return assignedChannel;
    //}

    //jassertfalse;
    //return -1;
}

int VoiceBank::channelOfVoice(const juce::MidiMessage& msg) const
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();
    return channelOfVoice(channel, note);
}

const VoiceBank::VoicePtr* VoiceBank::getExistingVoice(int index) const
{
    if (index >= 0 && index < voicesSize)
        return &voices[index];
    return nullptr;
}

void VoiceBank::setVoiceInChannel(ChannelInfo chInfo, int index, MidiVoice& voice)
{
    jassert(chInfo.channelPtr != nullptr);
    jassert(index >= 0 && index < chInfo.voiceLimit);

    chInfo.getVoicePtr(index)->voice = voice;
    
}

void VoiceBank::setVoiceInChannel(int midiChannel, int index, MidiVoice& voice)
{
    jassert(midiChannel == 0 || ChannelInMidiRange(midiChannel));
    auto chInfo = ChannelInMidiRange(midiChannel) ? channelInfo[midiChannel - 1]
                                                  : stolenChannelInfo;

    return setVoiceInChannel(chInfo, index, voice);
}

int VoiceBank::stealExistingVoice(int index)
{
    if (index < 0 || index >= voicesSize)
    {
        return -1;
    }

    auto voicePtr = &voices[index];
    if (voicePtr->voice.isActive())
    {
        auto channel = voicePtr->voice.getAssignedChannel();
        removeVoiceFromChannel(channel, voicePtr->voice);
        addVoiceToChannel(0, voicePtr->voice);
        return channel;
    }

    return -1;
}

void VoiceBank::retriggerExistingVoice(int index, int assignChannel)
{
    if (index >= 0)
    {
        auto retriggerVoicePtr = &voices[index];
        if (retriggerVoicePtr->voice.isValid())
        {
            int removedIndex = removeVoiceFromChannel(0, retriggerVoicePtr->voice);
            setVoiceInChannel(assignChannel, removedIndex, retriggerVoicePtr->voice);
        }
    }
}

int VoiceBank::addVoiceToChannel(ChannelInfo& chInfo, MidiVoice& voice)
{
    jassert(chInfo.channelPtr != nullptr);
    for (int i = 0; i < chInfo.voiceLimit; i++)
    {
        if (chInfo.getVoicePtr(i)->voice.isInvalid())
        {
            setVoiceInChannel(chInfo, i, voice);
            chInfo.numVoices++;
            return i;
        }
    }

    return -1;
}

int VoiceBank::addVoiceToChannel(int midiChannel, MidiVoice& voice)
{
    jassert(midiChannel == 0 || ChannelInMidiRange(midiChannel));
    auto chInfo = ChannelInMidiRange(midiChannel) ? channelInfo[midiChannel - 1]
                                                  : stolenChannelInfo;

    auto assignedIndex = addVoiceToChannel(chInfo, voice);
    if (assignedIndex >= 0)
    {
        voice.reassignChannel(midiChannel);
    }

    return assignedIndex;
}

int VoiceBank::removeVoiceFromChannel(ChannelInfo& chInfo, MidiVoice& voice)
{
    jassert(chInfo.channelPtr != nullptr);
    for (int i = 0; i < chInfo.voiceLimit; i++)
    {
        auto chVoicePtr = &chInfo.channelPtr[i];
        if (chVoicePtr->voice == voice)
        {
            chVoicePtr->voice = MidiVoice();
            chInfo.numVoices--;
            return i;
        }
    }

    return -1;
}

int VoiceBank::removeVoiceFromChannel(int midiChannel, MidiVoice& voice)
{
    jassert(midiChannel == 0 || ChannelInMidiRange(midiChannel));
    auto chInfo = ChannelInMidiRange(midiChannel) ? channelInfo[midiChannel - 1]
                                                  : stolenChannelInfo;
    
    return removeVoiceFromChannel(chInfo, voice);
}

const MidiVoice* VoiceBank::findChannelAndAddVoice(int midiChannel, int midiNote, juce::uint8 velocity)
{
    int newChannel = findNextVoiceChannel();

    if (numActiveVoices() < getVoiceLimit())
    {
        auto newVoice = MidiVoice(midiChannel, midiNote, velocity, newChannel, nullptr); // tuner
        int index = addVoiceToChannel(newChannel, newVoice);
        return &voices[index].voice;
    }


    if (ChannelInMidiRange(newChannel))
    {
        lastChannelAssigned = newChannel;

        // See if channel is occupied
        // TODO poly channel mode
        auto chInfo = channelInfo[newChannel - 1];
        if (chInfo.isFull())
        {
            auto voiceIndex = getNextVoiceIndexToSteal();
            stealExistingVoice(voiceIndex);

            auto newVoice = MidiVoice(midiChannel, midiNote, velocity, newChannel, nullptr); // tuner
            voices[voiceIndex] = VoicePtr{ newVoice };
        }
        jassertfalse;
        //voiceWatchers.call(&VoiceBank::Watcher::voiceAdded, *newVoice);
    }

    return nullptr;
}

MidiVoice VoiceBank::removeVoice(int index)
{
    jassert(index >= 0 && index < maxVoiceLimit);
    if (index >= 0 && index < maxVoiceLimit)
    {
        auto voice = &voices[index].voice;
        removeVoiceFromChannel(voice->getAssignedChannel(), *voice);

        // Only retrigger if an active voice was removed
        if (voice->isActive())
        {
            auto retriggerIndex = getNextVoiceToRetrigger();
            retriggerExistingVoice(retriggerIndex, voice->getAssignedChannel());
        }

        auto voiceCopy = *voice;
        voices[index] = VoicePtr();

        // Monophonic mode does not send note offs per note on, so send note off to proper channel
        if (voiceCopy.getAssignedChannel() == 0 && channelMode == Everytone::ChannelMode::Monophonic)
            voiceCopy.reassignChannel(lastChannelAssigned);

        //voiceWatchers.call(&VoiceBank::Watcher::voiceRemoved, voiceCopy);

        return voiceCopy;
    }
    return MidiVoice();
}

const MidiVoice* VoiceBank::getVoice(MidiVoice& voice)
{
    auto channel = voice.getMidiChannel();
    auto note = voice.getMidiNote();

    auto voiceIndex = indexOfVoice(voice.getMidiChannel(), voice.getMidiNote());
    if (voiceIndex >= 0 && voiceIndex < maxVoiceLimit)
    {
        auto voicePtr = getExistingVoice(voiceIndex);
        if (voicePtr == nullptr)
            return nullptr;
        return &voicePtr->voice;
    }

    return nullptr;
}

const MidiVoice* VoiceBank::getVoice(const juce::MidiMessage& msg)
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();

    // See if voice exists
    auto voiceIndex = getVoiceIndexFromInputMap(channel, note);
    if (voiceIndex >= 0)
        return &voices[voiceIndex].voice;

    // Add new voice
    auto velocity = msg.getVelocity();
    return findChannelAndAddVoice(channel, note, velocity);
}

int VoiceBank::numAllVoices() const
{
    return numVoices;
}

MidiVoice VoiceBank::removeVoice(int midiChannel, int midiNote)
{
    auto index = indexOfVoice(midiChannel, midiNote);
    return removeVoice(index);
}

MidiVoice VoiceBank::removeVoice(const juce::MidiMessage& msg)
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();
    return removeVoice(channel, note);
}

MidiVoice VoiceBank::removeVoice(const MidiVoice* voice)
{
    auto index = indexOfVoice(voice);
    return removeVoice(index);
}

void VoiceBank::clearVoices(juce::Array<MidiVoice>& voiceArray)
{
    for (auto voice : voiceArray)
    {
        removeVoice(&voice);
    }
}

void VoiceBank::clearAllVoices()
{
    auto allVoices = getAllVoices();
    clearVoices(allVoices);
//
//    auto emptyVoice = VoicePtr();
//
//    for (int v = 0; v < voicesSize; v++)
//    {
//        auto voice = &voices[v].voice;
//        if (voice->isValid())
//        {
//            removeVoiceFromChannel(voice->getAssignedChannel(), voice);
//        }
//        
//        voices[v] = emptyVoice;
//    }
//
//    for (int s = 0; s < stolenChannelInfo.voiceLimit; s++)
//    {
//        auto voice = &stolenVoices[s];
//        if (voice->voice.isValid())
//        {
//            // TODO remove from stolen voices
//        }
//
//        stolenVoices[s] = emptyVoice;
//    }
}


bool VoiceBank::channelIsFree(int midiChannel, MidiPitch pitchToAssign) const
{
    if (!ChannelInMidiRange(midiChannel))
    {
        jassertfalse;
        return false;
    }

    auto chInfo = channelInfo[midiChannel - 1];
    bool notAvailable = chInfo.disabled;

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
        auto channelVoices = getVoicesInChannel(midiChannel);

        // TODO Poly channel mode
        if (channelVoices.size() == 0)
            return true;
    }

    return false;
}

void VoiceBank::setChannelsDisabled(juce::Array<bool> channelsDisabled)
{
    for (int ch = 0; ch < channelsDisabled.size(); ch++)
        channelInfo.getReference(ch).disabled = channelsDisabled[ch];

    updateVoiceLimitCache();
}

void VoiceBank::setChannelMode(Everytone::ChannelMode mode)
{
    channelMode = mode;
    juce::Logger::writeToLog("ChannelMode set to " + Everytone::getName(channelMode));
    updateVoiceLimitCache();
}

void VoiceBank::setMpeZone(Everytone::MpeZone zone)
{
    mpeZone = zone;
    juce::Logger::writeToLog("MPE Zone set to " + Everytone::getName(mpeZone));
    updateVoiceLimitCache();
}

void VoiceBank::setNotePriority(Everytone::NotePriority notePriorityIn)
{
    notePriority = notePriorityIn;
    juce::Logger::writeToLog("NotePriority set to " + Everytone::getName(notePriority));
}

int VoiceBank::findLowestVoiceIndex(bool active) const
{
    if (numActiveVoices() == 0)
        return 0;

    auto lowestPitch = voices[0].voice.getCurrentPitch();
    int lowestIndex = 0;
    for (int i = 1; i < voicesSize; i++)
    {
        auto voice = &voices[i].voice;
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

int VoiceBank::findHighestVoiceIndex(bool active) const
{
    if (numActiveVoices() == 0)
        return 0;

    auto highestPitch = voices[0].voice.getCurrentPitch();
    int highestIndex = 0;
    for (int i = 1; i < voicesSize; i++)
    {
        auto voice = &voices[i].voice;
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

int VoiceBank::findOldestVoiceIndex(bool active) const
{
    for (int i = 0; i < voicesSize; i++)
    {
        auto voice = &voices[i].voice;
        if (active && !voice->isActive())
            continue;

        if (!active && voice->isActive())
            continue;

        return indexOfVoice(voice);
    }

    return 0;
}

int VoiceBank::findMostRecentVoiceIndex(bool active) const
{
    for (int i = 0; i < voicesSize; i++)
    {
        int index = voicesSize - i - 1;
        auto voice = &voices[index].voice;
        if (active && !voice->isActive())
            continue;

        if (!active && voice->isActive())
            continue;

        return indexOfVoice(voice);
    }

    return 0;
}

int VoiceBank::nextAvailableChannel() const
{
    // TODO Poly channel mode

    for (int ch = 1; ch < MAX_CHANNELS; ch++)
    {
        if (channelIsFree(ch))
            return ch;
    }
    return -1;
}

int VoiceBank::nextRoundRobinChannel() const
{
    // TODO Poly channel mode

    auto i = (lastChannelAssigned + 1) % 16;

    int channelsChecked = 1;
    while (channelsChecked < MAX_CHANNELS)
    {
        auto ch = i + 1;
        if (channelIsFree(ch))
            return ch;

        i = ch % 16;
        channelsChecked++;
    }

    return -1;
}

int VoiceBank::getNextVoiceIndexToSteal() const
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

int VoiceBank::getNextVoiceToRetrigger() const
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

int VoiceBank::findNextVoiceChannel(MidiPitch pitchOfNewVoice) const
{
    auto newVoiceState = getNewVoiceState();

    int newChannel = -1;

    switch (newVoiceState)
    {
    case VoiceBank::NewVoiceState::Monophonic:
        newChannel = lastChannelAssigned;
        if (!ChannelInMidiRange(newChannel))
            newChannel = nextAvailableChannel();
        break;

    case VoiceBank::NewVoiceState::Overflow:
    {
        auto stealIndex = getNextVoiceIndexToSteal();
        auto stealVoicePtr = getExistingVoice(stealIndex);
        if (stealVoicePtr != nullptr)
        {
            newChannel = stealVoicePtr->voice.getAssignedChannel();
            //stealExistingVoice(stealIndex);
        }
        break;
    }

    case VoiceBank::NewVoiceState::Normal:
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


int VoiceBank::midiNoteIndex(int midiChannel, int midiNote) const
{
    return (midiChannel - 1) * 128 + midiNote;
}

int VoiceBank::getMapNoteIndex(int midiNote) const
{
    return midiNote * mapIndicesPerNote;
}

int VoiceBank::getVoiceIndexFromInputMap(int midiChannel, int midiNote) const
{
    bool validNote = midiChannel >= 0 && midiChannel <= 16 && midiNote >= 0 && midiNote < 128;
    jassert(validNote);

    if (!validNote)
        return -1;

    auto mapIndex = getMapNoteIndex(midiNote) + midiChannel;
    auto noteIndex = inputNoteVoiceIndexMap[mapIndex];
    return noteIndex;
}

int VoiceBank::indexOfVoice(int midiChannel, int midiNote) const
{
    //auto midiIndex = midiNoteIndex(midiChannel, midiNote);
    //for (int i = 0; i < voicesSize; i++)
    //{
    //    auto voice = getExistingVoice(i);
    //    if (voice == nullptr || !voice->voice.isValid())
    //        continue;

    //    if (voice->voice.getMidiNoteIndex() == midiIndex)
    //        return i;
    //}
    //return -1;
    return getVoiceIndexFromInputMap(midiChannel, midiNote);
}

int VoiceBank::indexOfVoice(const MidiVoice* voice) const
{
    //for (int i = 0; i < voicesSize; i++)
    //    if (getExistingVoice(i) == voice)
    //        return i;
    //return -1;

    return indexOfVoice(voice->getMidiChannel(), voice->getMidiNote());
}

int VoiceBank::effectiveVoiceLimit() const
{
    int numDisabled = 0;
    for (auto chInfo : channelInfo)
        numDisabled += (chInfo.disabled) ? 1 : 0;

    int limit = maxVoiceLimit - numDisabled;
    limit -= (mpeZone == Everytone::MpeZone::Omnichannel) ? 0 : 1;
    return limit;
}

void VoiceBank::updateVoiceLimitCache()
{
    // TODO poly channel mode
    voiceLimit = effectiveVoiceLimit();
}

//int VoiceBank::numVoicesAvailable() const
//{
//    // TODO poly channel mode
//    switch (channelMode)
//    {
//    case Everytone::ChannelMode::Monophonic:
//        return 1;
//
//    default:
//        return voiceLimit - activevoicesSize;
//    }
//}
