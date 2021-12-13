/*
  ==============================================================================

    MidiVoiceController.cpp
    Created: 12 Dec 2021 2:14:00pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "MidiVoiceController.h"

MidiVoiceController::MidiVoiceController(MappedTuningController& tuningControllerIn)
    : tuningController(tuningControllerIn)
{
    voices.resize(MULTIMAPPER_MAX_VOICES);
    voices.fill(MidiVoice());
}

MidiVoiceController::~MidiVoiceController()
{

}


const MidiVoice* MidiVoiceController::getVoice(int index)
{
    return &voices.getReference(index);
}


const MidiVoice* MidiVoiceController::getVoice(int midiChannel, int midiNote) const
{
    auto voiceIndex = indexOfVoice(midiChannel, midiNote);
    if (voiceIndex >= 0)
        return getVoice(voiceIndex);
}

const MidiVoice* MidiVoiceController::getVoice(const juce::MidiMessage& msg) const
{
    auto channel = msg.getChannel();
    auto note = msg.getNoteNumber();
    return getVoice(channel, note);
}

int MidiVoiceController::numVoices() const
{
    int num = 0;
    for (auto voice : voices)
    {
        if (voice.getAssignedChannel() >= 0)
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
    auto newIndex = nextAvailableVoiceIndex();

    if (newIndex >= 0)
    {
        auto newVoice = MidiVoice(midiChannel, midiNote, velocity, newIndex + 1, tuningController.getTuner());
        voices.set(newIndex, newVoice);
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
    auto voice = voices[index];
    voices.set(index, MidiVoice());
    return voice;
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

void MidiVoiceController::setChannelDisabled(int midiChannel, bool disabled)
{
    midiChannelDisabled.set(midiChannel - 1, disabled);
}

int MidiVoiceController::nextAvailableVoiceIndex()
{
    for (int i = 0; i < voices.size(); i++)
        if (voices[i].getAssignedChannel() < 1 && !midiChannelDisabled[i])
            return i;
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
        if (voices[i].getMidiNoteIndex() == midiIndex)
            return i;
    return -1;
}

int MidiVoiceController::indexOfVoice(const MidiVoice* voice) const
{
    for (int i = 0; i < voices.size(); i++)
        if (&voices.getReference(i) == voice)
            return i;
    return -1;
}
