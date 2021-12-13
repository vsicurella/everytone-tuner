/*
  ==============================================================================

    MidiVoice.cpp
    Created: 12 Dec 2021 2:15:00pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "MidiVoice.h"


MidiVoice::MidiVoice(int channelIn, int noteIn, juce::uint8 velocityIn, int assignedChannelIn, std::shared_ptr<MidiNoteTuner> tunerIn)
    : midiChannel(channelIn),
      midiNote(noteIn),
      velocity(velocityIn),
      assignedChannel(assignedChannelIn),
      tuner(tunerIn)
{
    update();
}

void MidiVoice::updateMapping()
{
    auto previousMappedNote = currentMappedNote;
    currentMappedNote = tuner->getNoteMapping(midiChannel, midiNote);

}

void MidiVoice::updatePitch()
{
    auto previousPitch = currentPitch;
    currentPitch = tuner->getMidiPitch(currentMappedNote);
}

void MidiVoice::update()
{
    updateMapping();
    updatePitch();
}

void MidiVoice::updateAftertouch(juce::uint8 aftertouchIn)
{
    aftertouch = aftertouchIn;
}

int MidiVoice::findControllerIndex(int number)
{
    for (int i = 0; i < controllers.size(); i++)
        if (controllers[i].number == number)
            return i;
    
    return -1;
}

void MidiVoice::updateController(int number, juce::uint8 value)
{
    auto index = findControllerIndex(number);
    if (index >= 0)
        controllers.getReference(index).value = value;
}

juce::uint8 MidiVoice::getControllerValue(int number)
{
    auto index = findControllerIndex(number);
    if (index >= 0)
        return controllers[index].value;
    return 0;
}

juce::MidiMessage MidiVoice::getNoteOn() const
{
    return juce::MidiMessage::noteOn(assignedChannel, currentPitch.coarse, velocity);
}

juce::MidiMessage MidiVoice::getPitchbend() const
{
    return juce::MidiMessage::pitchWheel(assignedChannel, currentPitch.pitchbend);
}

juce::MidiMessage MidiVoice::getAftertouch(int value) const
{
    return juce::MidiMessage::aftertouchChange(assignedChannel, currentPitch.coarse, value);
}

juce::MidiMessage MidiVoice::getNoteOff() const
{
    return juce::MidiMessage::noteOff(assignedChannel, currentPitch.coarse);
}

void MidiVoice::mapMidiMessage(juce::MidiMessage msg) const
{
    msg.setChannel(assignedChannel);
    msg.setNoteNumber(currentPitch.coarse);
}
