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

void MidiBrain::processMidi(juce::MidiBuffer& buffer)
{
    juce::MidiBuffer processedBuffer;
    int sampleOffset = 0;
    for (auto metadata : buffer)
    {
        auto msg = metadata.getMessage();

        if (msg.isNoteOn())
        {
            int pitchbend = tuner->mapNoteAndPitchbend(msg);

            if (pitchbend != 8192)
            {
                // Create and add pitchbend message
                auto pbmsg = juce::MidiMessage::pitchWheel(msg.getChannel(), pitchbend);
                auto sample = (metadata.samplePosition == 0) ? 0 : metadata.samplePosition - 1;

#if JUCE_DEBUG
                juce::Logger::writeToLog(pbmsg.getDescription());
#endif

                processedBuffer.addEvent(msg, sample);
            }
        }
        // Don't have to find pitchbend for Note Off or Aftertouch
        else if (msg.isNoteOff() || msg.isAftertouch())
        {
            tuner->mapMidiNote(msg);
        }
      
        
        processedBuffer.addEvent(msg, metadata.samplePosition);
    }

    buffer.swapWith(processedBuffer);
}

void MidiBrain::preTuningChange(const Tuning& tuning)
{

}

void MidiBrain::postTuningChange()
{

}