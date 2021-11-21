/*
  ==============================================================================

    MidiBrain.cpp
    Created: 19 Nov 2021 6:23:15pm
    Author:  vincenzo

  ==============================================================================
*/

#include "MidiBrain.h"

MidiBrain::MidiBrain()
{
    auto standardTuning = Tuning::StandardTuning();
    oldTuning.reset(new Tuning(standardTuning));
    newTuning.reset(new Tuning(standardTuning));

    mapper.reset(new MidiNoteMapper());
    // tuner.reset(new MidiNoteTuner(oldTuning, newTuning));
}

MidiBrain::~MidiBrain()
{
    // tuner       = nullptr;
    mapper      = nullptr;
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
            auto ch = msg.getChannel();
            auto note = msg.getNoteNumber();

            // Map channel and note to tuning index
            auto mapped = mapper->getNoteAt(ch, note);

            // Get new note pitch from tuning index
            auto newCents = newTuning->getNoteInCents(mapped.index);
            auto newNote = newTuning->closestNoteToCents(newCents);            
            
            // Apply changes
            msg.setNoteNumber(newNote);

            // Get note to tune from old tuning, and pitchbend amount to adjust by
            auto oldNote = oldTuning->closestNoteToCents(newCents);
            auto oldCents = oldTuning->getNoteInCents(oldNote);
            auto pitchbend = MidiNoteTuner::pitchbendAmount(pitchbendRange, oldCents / 100.0, newCents / 100.0);

            if (pitchbend != 8192)
            {
                // Create and add pitchbend message
                auto pbmsg = juce::MidiMessage::pitchWheel(ch, pitchbend);
                auto sample = (metadata.samplePosition == 0) ? 0 : metadata.samplePosition - 1;

#if JUCE_DEBUG
                juce::Logger::writeToLog(pbmsg.getDescription());
#endif

                processedBuffer.addEvent(msg, sample);
            }
        }

#if JUCE_DEBUG
        juce::Logger::writeToLog(msg.getDescription());
#endif
        
        processedBuffer.addEvent(msg, metadata.samplePosition);
    }

    buffer.swapWith(processedBuffer);
}