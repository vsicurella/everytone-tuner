/*
  ==============================================================================

    MidiBrain.cpp
    Created: 19 Nov 2021 6:23:15pm
    Author:  vincenzo

  ==============================================================================
*/

#include "MidiBrain.h"

#include "tests/TestCommon.h"

MidiBrain::MidiBrain()
{
    auto standardTuning = Tuning::StandardTuning();
    tuningSource.reset(new Tuning(standardTuning));

    tuningTarget.reset(new Tuning(standardTuning));
    tuner.reset(new MidiNoteTuner(tuningSource.get(), tuningTarget.get()));

    channelsInUse.resize(16);
    channelsInUse.fill(-1);
}

MidiBrain::~MidiBrain()
{
    tuner       = nullptr;
    tuningSource   = nullptr;
    tuningTarget   = nullptr;
}

void MidiBrain::setTuningSource(const Tuning& tuning)
{
    preTuningChange(tuning);
    tuningSource.reset(new Tuning(tuning));
    postTuningChange();
}

void MidiBrain::setTuningTarget(const Tuning& tuning)
{
    preTuningChange(tuning);
    tuningTarget.reset(new Tuning(tuning));
    postTuningChange();
}

void MidiBrain::setNoteMap(const Keytographer::TuningTableMap& map)
{
    preMapChange(map);
    noteMap.reset(new Keytographer::TuningTableMap(map));
    postMapChange();
}

void MidiBrain::processMidi(juce::MidiBuffer& buffer)
{
    juce::MidiBuffer processedBuffer;
    int sampleOffset = 0;
    for (auto metadata : buffer)
    {
        auto msg = metadata.getMessage();

        auto status = msg.getRawData()[0];
        bool isVoice = status >= 0x80 && status < 0xb0;
       
        if (isVoice)
        {
            auto oldnote = msg.getNoteNumber();
            double semitones = tuner->mapMidiNote(msg);
            //juce::Logger::writeToLog("Remapped note " + juce::String(oldnote) + " to " + juce::String(msg.getNoteNumber()));

            if (msg.isNoteOn())
            {
                int newChannel = nextAvailableChannel();

                if (newChannel < 0)
                {
                    //juce::Logger::writeToLog("No channels available");
                    continue;
                }

                //juce::Logger::writeToLog("channel set to " + juce::String(newChannel +1));
                msg.setChannel(newChannel + 1);
                channelsInUse.set(newChannel, msg.getNoteNumber());

                if (abs(semitones) >= 1e-6)
                {
                    auto pitchbend = tuner->semitonesToPitchbend(semitones);

                    // Create and add pitchbend message
                    auto pbmsg = juce::MidiMessage::pitchWheel(msg.getChannel(), pitchbend);
                    //auto sample = (metadata.samplePosition == 0) ? 0 : metadata.samplePosition - 1;

//#if JUCE_DEBUG
                    //juce::Logger::writeToLog(pbmsg.getDescription());
//#endif

                    processedBuffer.addEvent(pbmsg, -1);
                }
            }
            else
            {
                int noteChannel = channelOfActiveNote(msg.getNoteNumber());
                if (noteChannel < 0)
                {
                    //juce::Logger::writeToLog("got voice with no note on");
                    continue;
                }

                msg.setChannel(noteChannel + 1);

                if (msg.isNoteOff())
                {
                    channelsInUse.set(noteChannel, -1);
                    //juce::Logger::writeToLog("note off " + juce::String(msg.getNoteNumber()) + " channel " + juce::String(noteChannel+1));
                }
            }
        }
        
        processedBuffer.addEvent(msg, -1);
    }

    buffer.swapWith(processedBuffer);
}

int MidiBrain::nextAvailableChannel()
{
    for (int ch = 0; ch < 16; ch++)
    {
        if (channelsInUse[ch] < 0)
            return ch;
    }

    return -1;
}

int MidiBrain::channelOfActiveNote(int noteNumber)
{
    for (int ch = 0; ch < 16; ch++)
    {
        if (channelsInUse[ch] == noteNumber)
            return ch;
    }
    return -1;
}

void MidiBrain::preTuningChange(const Tuning& tuning)
{
    // send note offs
}

void MidiBrain::postTuningChange()
{
    tuner->setSourceTuning(tuningSource.get());
    tuner->setTargetTuning(tuningTarget.get());
}

void MidiBrain::preMapChange(const Keytographer::TuningTableMap& map)
{
    // send note offs
}

void MidiBrain::postMapChange()
{
    tuner->setTuningTableMap(noteMap.get());
}

