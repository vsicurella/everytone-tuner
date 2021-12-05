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
    oldTuning.reset(new Tuning(standardTuning));

    Tuning::EqualTemperamentDefinition def31;
    def31.divisions = 31;

    newTuning.reset(new Tuning(def31));
    tuner.reset(new MidiNoteTuner(oldTuning.get(), newTuning.get()));
}

MidiBrain::~MidiBrain()
{
    tuner       = nullptr;
    oldTuning   = nullptr;
    newTuning   = nullptr;
}

void MidiBrain::processMidi(juce::MidiBuffer& buffer)
{
    juce::MidiBuffer processedBuffer;
    int sampleOffset = 0;
    for (auto metadata : buffer)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOnOrOff() || msg.isAftertouch())
        {
            int pitchbend = tuner->tuneNoteAndGetPitchbend(msg);

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
        
        processedBuffer.addEvent(msg, metadata.samplePosition);
    }

    buffer.swapWith(processedBuffer);
}