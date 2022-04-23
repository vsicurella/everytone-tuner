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
    auto emptyVoice = VoicePtr();
    auto stolenVoicesSize = MAX_VOICES_PER_CHANNEL;
    for (int i = 0; i < stolenVoicesSize; i++)
    {
        stolenVoices[i] = emptyVoice;
    }

    stolenChannelInfo = ChannelInfo(0, stolenVoicesSize, stolenVoices);

    // VoiceBank::inputNoteVoiceIndexMap actually uses interleaved channels, unlike VoiceBank::voices
    // Doesn't matter here since we're just clearing them
    auto emptyChVoice = ChannelVoicePtr();
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
            inputNoteVoiceIndexMap[voiceIndex] = emptyChVoice;
            voiceIndex++;
        }
    }

    voicesSize = voiceIndex;

    noteMapSize = voicesSize;
    mapIndicesPerNote = MAX_CHANNELS;
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

void VoiceBank::setMidiNoteTuner(std::shared_ptr<MidiNoteTuner>& newTuner)
{
    tuner = newTuner;
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
    auto mapIndex = getMapNoteIndex(midiNote);
    for (int i = 0; i < mapIndicesPerNote; i++)
    {
        auto chVoicePtr = inputNoteVoiceIndexMap[mapIndex + i];
        if (chVoicePtr.isInvalid())
            continue;

        auto voicePtr = chVoicePtr.voicePtr;
        if (voicePtr->voice.isInvalid())
        {
            DBG("Did not expect mapped voice to exist; Note " + String(midiNote) + " Channel " + String(midiChannel));
            continue;
        }

        auto assignedChannel = voicePtr->voice.getAssignedChannel();
        if (assignedChannel >= 0)
            return assignedChannel;

        jassertfalse;
        DBG("Did not expect valid voice to have an invalid assigned channel; Note " + String(midiNote) + " Channel " + String(midiChannel));
        break;
    }

    jassertfalse;
    return -1;
}

int VoiceBank::channelOfVoice(const juce::MidiMessage& msg) const
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();
    return channelOfVoice(channel, note);
}

const MidiVoice* VoiceBank::getExistingVoice(int index) const
{
    if (index >= 0 && index < voicesSize)
        return &voices[index].voice;
    return nullptr;
}

const VoiceBank::VoicePtr* VoiceBank::getExistingVoicePtr(int index) const
{
    if (index >= 0 && index < voicesSize)
        return &voices[index];
    return nullptr;
}

void VoiceBank::setVoiceInChannel(ChannelInfo& chInfo, int index, MidiVoice& voice)
{
    jassert(chInfo.channelPtr != nullptr);
    jassert(index >= 0 && index < chInfo.voiceLimit);

    auto voicePtr = chInfo.getVoicePtr(index);
    voicePtr->voice = voice;

    auto chVoicePtr = ChannelVoicePtr{ voicePtr, &chInfo, index };
    setInputMapVoice(chVoicePtr);
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

int VoiceBank::removeVoiceFromChannel(ChannelInfo& chInfo, const MidiVoice& voice)
{
    jassert(chInfo.channelPtr != nullptr);
    auto inputNote = voice.getMidiNote();
    auto inputChannel = voice.getMidiChannel();

    for (int i = 0; i < chInfo.voiceLimit; i++)
    {
        auto voicePtr = chInfo.getVoicePtr(i);
        if (voicePtr->voice == voice)
        {
            voicePtr->voice = MidiVoice();
            chInfo.numVoices--;

            auto mapIndex = getMapNoteIndex(inputNote) + inputChannel - 1;
            inputNoteVoiceIndexMap[mapIndex] = ChannelVoicePtr();
            return i;
        }
    }

    return -1;
}

int VoiceBank::removeVoiceFromChannel(int midiChannel, const MidiVoice& voice)
{
    jassert(midiChannel == 0 || ChannelInMidiRange(midiChannel));
    auto chInfo = ChannelInMidiRange(midiChannel) ? channelInfo[midiChannel - 1]
                                                  : stolenChannelInfo;
    
    return removeVoiceFromChannel(chInfo, voice);
}

int VoiceBank::removeVoiceFromChannel(ChannelVoicePtr& chVoicePtr)
{
    // annoying but workaround const ChannelInfo*
    return removeVoiceFromChannel(chVoicePtr.channel->id, chVoicePtr.voicePtr->voice);
}

VoiceBank::ChannelVoicePtr VoiceBank::findVoice(const MidiVoice& voiceToFind) const
{
    for (int i = 0; i < channelInfo.size(); i++)
    {
        auto ch = &channelInfo.getReference(i);
        if (ch->disabled)
            continue;

        int numFound = 0;
        int chIndex = 0;
        while (numFound < ch->numVoices && chIndex < ch->voiceLimit)
        {
            auto voice = ch->getVoicePtr(chIndex);
            if (voice != nullptr)
            {
                auto result = ChannelVoicePtr{ voice, ch, chIndex };
                return result;
            }
        }
    }

    return ChannelVoicePtr();
}

const MidiVoice* VoiceBank::findChannelAndAddVoice(NewVoiceState state, int midiChannel, int midiNote, juce::uint8 velocity)
{
    if (state == NewVoiceState::Overflow)
    {
        auto findResult = findNextVoiceToSteal();
        if (!findResult.isValid())
        {
            // do some NUM VOICES == VOICE LIMIT check
            return nullptr;
        }

        auto voiceIndex = findResult.getVoiceIndex();
        auto newChannelNum = stealExistingVoice(voiceIndex);
        lastChannelAssigned = newChannelNum;

        jassert(newChannelNum >= 1);

        auto newVoice = MidiVoice(midiChannel, midiNote, velocity, newChannelNum, tuner);
        voices[voiceIndex] = VoicePtr{ newVoice };
        return getExistingVoice(voiceIndex);
    }

    auto newChannelNum = findNextVoiceChannel(state);
    jassert(newChannelNum != 0);
    if (newChannelNum < 0)
        return nullptr;

    auto newChannel = &channelInfo.getReference(newChannelNum - 1);
    if (newChannel->isValid())
    {
        lastChannelAssigned = newChannel->id;
        auto newVoice = MidiVoice(midiChannel, midiNote, velocity, newChannel->id, tuner);
        auto channelIndex = addVoiceToChannel(*newChannel, newVoice);
        return &(newChannel->getVoicePtr(channelIndex)->voice);
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
            auto findResult = findNextVoiceToRetrigger();
            if (!findResult.isValid())
            {
                jassertfalse;
                return MidiVoice();
            }

            auto retriggerIndex = findResult.getVoiceIndex();
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
//
//const MidiVoice* VoiceBank::getVoice(MidiVoice& voice)
//{
//    auto channel = voice.getMidiChannel();
//    auto note = voice.getMidiNote();
//
//    auto voiceIndex = indexOfVoice(voice.getMidiChannel(), voice.getMidiNote());
//    if (voiceIndex >= 0 && voiceIndex < maxVoiceLimit)
//    {
//        auto voicePtr = getExistingVoicePtr(voiceIndex);
//        if (voicePtr == nullptr)
//            return nullptr;
//        return &voicePtr->voice;
//    }
//        
//    return nullptr;
//}

const MidiVoice* VoiceBank::getVoice(const juce::MidiMessage& msg)
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();

    // See if voice exists
    auto voiceChPtr = getVoiceFromInputMap(channel, note);
    if (voiceChPtr.isValid())
        return &voiceChPtr.voicePtr->voice;

    auto voiceState = getNewVoiceState();
    if (voiceState == NewVoiceState::Overflow && notePriority == Everytone::NotePriority::Ignore)
    {
        // Ignore voice
        return nullptr;
    }
    
    // Add to a channel or steal voice
    auto velocity = msg.getVelocity();
    return findChannelAndAddVoice(voiceState, channel, note, velocity);
}

int VoiceBank::numAllVoices() const
{
    return numVoices;
}

MidiVoice VoiceBank::removeVoice(int midiChannel, int midiNote)
{
    auto chVoicePtr = getVoiceFromInputMap(midiChannel, midiNote);
    if (chVoicePtr.isInvalid())
    {
        jassertfalse;
        return MidiVoice();
    }

    auto voice = chVoicePtr.voicePtr->voice;
    removeVoiceFromChannel(chVoicePtr);
    return voice;
    
    //auto index = indexOfVoice(midiChannel, midiNote);
    //return removeVoice(voicePtr->voice);
}

MidiVoice VoiceBank::removeVoice(const juce::MidiMessage& msg)
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();
    return removeVoice(channel, note);
}

//MidiVoice VoiceBank::removeVoice(const MidiVoice* voice)
//{
//    auto index = indexOfVoice(voice);
//    return removeVoice(index);
//}
//
//void VoiceBank::clearVoices(juce::Array<MidiVoice>& voiceArray)
//{
//    for (auto voice : voiceArray)
//    {
//        removeVoice(&voice);
//    }
//}

void VoiceBank::clearAllVoices()
{
    auto emptyVoice = VoicePtr();

    for (int v = 0; v < voicesSize; v++)
    {
        auto voice = &voices[v].voice;
        if (voice->isValid())
        {
            removeVoiceFromChannel(voice->getAssignedChannel(), *voice);
        }
        
        voices[v].voice = MidiVoice();
    }

    for (int s = 0; s < stolenChannelInfo.voiceLimit; s++)
    {
        auto voice = &stolenVoices[s];
        if (voice->voice.isValid())
        {
            // TODO remove from stolen voices
        }

        stolenVoices[s] = emptyVoice;
    }
}


bool VoiceBank::channelIsFree(int midiChannel, MidiPitch pitchToAssign) const
{
    if (!ChannelInMidiRange(midiChannel))
    {
        jassertfalse;
        return false;
    }

    auto chInfo = channelInfo[midiChannel - 1];
    bool notAvailable = (chInfo.disabled || chInfo.numVoices >= MAX_VOICES_PER_CHANNEL);

    switch (mpeZone)
    {
    case Everytone::MpeZone::Lower:
        notAvailable |= midiChannel == 1;
        break;

    case Everytone::MpeZone::Upper:
        notAvailable |= midiChannel == 16;
        break;
    }

    return !notAvailable;
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
    updateVoiceLimitCache();
}

void VoiceBank::setMpeZone(Everytone::MpeZone zone)
{
    mpeZone = zone;
    updateVoiceLimitCache();
}

void VoiceBank::setNotePriority(Everytone::NotePriority notePriorityIn)
{
    notePriority = notePriorityIn;
}

// TODO: implement these by channel iteration

VoiceBank::ChannelVoicePtr VoiceBank::findLowestVoiceIndex(bool active) const
{
    if (numActiveVoices() == 0)
        return ChannelVoicePtr();

    auto lowestPitch = voices[0].voice.getCurrentPitch();
    int lowestIndex = 0;
    const MidiVoice* lowestVoice = nullptr;
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
            lowestVoice = voice;
        }
    }

    return findVoice(*lowestVoice);
}

VoiceBank::ChannelVoicePtr VoiceBank::findHighestVoiceIndex(bool active) const
{
    if (numActiveVoices() == 0)
        return ChannelVoicePtr();

    auto highestPitch = voices[0].voice.getCurrentPitch();
    int highestIndex = 0;
    const MidiVoice* highestVoice = nullptr;
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
            highestVoice = voice;
        }
    }

    return findVoice(*highestVoice);
}

VoiceBank::ChannelVoicePtr VoiceBank::findOldestVoiceIndex(bool active) const
{
    for (int i = 0; i < voicesSize; i++)
    {
        auto voice = &voices[i].voice;
        if (active && !voice->isActive())
            continue;

        if (!active && voice->isActive())
            continue;

        return findVoice(*voice);
    }

    return ChannelVoicePtr();
}

VoiceBank::ChannelVoicePtr VoiceBank::findMostRecentVoiceIndex(bool active) const
{
    for (int i = 0; i < voicesSize; i++)
    {
        int index = voicesSize - i - 1;
        auto voice = &voices[index].voice;
        if (active && !voice->isActive())
            continue;

        if (!active && voice->isActive())
            continue;

        return findVoice(*voice);
    }

    return ChannelVoicePtr();
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

VoiceBank::ChannelVoicePtr VoiceBank::findNextVoiceToSteal() const
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

    return ChannelVoicePtr();
}

VoiceBank::ChannelVoicePtr VoiceBank::findNextVoiceToRetrigger() const
{
    auto inactiveVoices = getVoicesInChannel(0);
    if (inactiveVoices.size() == 0)
        return ChannelVoicePtr();

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

    return ChannelVoicePtr();
}

int VoiceBank::findNextVoiceChannel(NewVoiceState state, MidiPitch pitchOfNewVoice) const
{
    int newChannel = -1;

    switch (state)
    {
    case VoiceBank::NewVoiceState::Monophonic:
        newChannel = lastChannelAssigned;
        if (!ChannelInMidiRange(newChannel))
            newChannel = nextAvailableChannel();
        break;

    case VoiceBank::NewVoiceState::Overflow:
    {
        auto stealResult = findNextVoiceToSteal();
        if (!stealResult.isValid())
        {
            jassertfalse;
            return -1;
        }

        auto stealIndex = stealResult.getVoiceIndex();
        auto stealVoice = getExistingVoice(stealIndex);
        if (stealVoice != nullptr)
        {
            newChannel = stealVoice->getAssignedChannel();
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

VoiceBank::ChannelVoicePtr VoiceBank::getVoiceFromInputMap(int midiChannel, int midiNote) const
{
    bool validNote = midiChannel >= 0 && midiChannel <= 16 && midiNote >= 0 && midiNote < 128;
    jassert(validNote);

    if (!validNote)
        return ChannelVoicePtr();

    auto mapIndex = getMapNoteIndex(midiNote) + midiChannel - 1;
    return inputNoteVoiceIndexMap[mapIndex];
}

void VoiceBank::setInputMapVoice(const ChannelVoicePtr& chVoicePtr)
{
    jassert(chVoicePtr.isValid());
    auto midiChannel = chVoicePtr.voicePtr->voice.getMidiChannel();
    auto midiNote = chVoicePtr.voicePtr->voice.getMidiNote();

    bool validNote = midiChannel >= 0 && midiChannel <= 16 && midiNote >= 0 && midiNote < 128;
    jassert(validNote);
    if (!validNote)
        return;

    auto mapIndex = getMapNoteIndex(midiNote) + midiChannel - 1;
    inputNoteVoiceIndexMap[mapIndex] = chVoicePtr;
}

//int VoiceBank::indexOfVoice(int midiChannel, int midiNote) const
//{
//    //auto midiIndex = midiNoteIndex(midiChannel, midiNote);
//    //for (int i = 0; i < voicesSize; i++)
//    //{
//    //    auto voice = getExistingVoicePtr(i);
//    //    if (voice == nullptr || !voice->voice.isValid())
//    //        continue;
//
//    //    if (voice->voice.getMidiNoteIndex() == midiIndex)
//    //        return i;
//    //}
//    //return -1;
//    auto voice = getVoiceFromInputMap(midiChannel, midiNote);
//    if (voice->voice.isValid() && voice->voice.getAssignedChannel() > 0)
//    auto ch = channelI
//    return 
//}

//int VoiceBank::indexOfVoice(const MidiVoice* voice) const
//{
//    //for (int i = 0; i < voicesSize; i++)
//    //    if (getExistingVoicePtr(i) == voice)
//    //        return i;
//    //return -1;
//
//    return indexOfVoice(voice->getMidiChannel(), voice->getMidiNote());
//}

int VoiceBank::effectiveVoiceLimit() const
{
    if (channelMode == Everytone::ChannelMode::Monophonic)
        return 1;

    int numDisabled = 0;
    for (int i = 0; i < channelInfo.size(); i++)
        numDisabled += (channelInfo[i].disabled) ? 1 : 0;

    int limit = maxVoiceLimit - numDisabled;
    limit -= (mpeZone == Everytone::MpeZone::Omnichannel) ? 0 : 1;
    return limit;
}

void VoiceBank::updateVoiceLimitCache()
{
    // TODO poly channel mode
    voiceLimit = effectiveVoiceLimit();
}

int VoiceBank::numVoicesAvailable() const
{
    int num = 0;
    // TODO poly channel mode
    switch (channelMode)
    {
    case Everytone::ChannelMode::Monophonic:
        num = 1;
        break;

    default:
    {
        int chVoices = 0;
        for (auto ch : channelInfo)
        {
            if (ch.disabled)
                continue;

            chVoices += ch.numVoices;
        }

        num = maxVoiceLimit - chVoices;
        break;
    }
    }

    return num;
}
