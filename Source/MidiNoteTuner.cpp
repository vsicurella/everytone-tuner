/*
  ==============================================================================

    MidiNoteTuner.cpp
    Created: 30 Nov 2019 5:11:32pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "MidiNoteTuner.h"

MidiNoteTuner::MidiNoteTuner(const Tuning& defaultTuning, const Tuning& newTuningIn)
	: originTuning(&defaultTuning), newTuning(&newTuningIn)
{
    
}

MidiNoteTuner::~MidiNoteTuner()
{

}

void MidiNoteTuner::setOriginTuning(const Tuning& originTuningIn)
{
	originTuning = &originTuningIn;
}

void MidiNoteTuner::setNewTuning(const Tuning& newTuningIn)
{
	newTuning = &newTuningIn;
}

juce::Array<int> MidiNoteTuner::getPitchbendTable() const
{
	return pitchbendTable;
}

int MidiNoteTuner::getPitchbendMax() const
{
    return pitchbendRange;
}

int MidiNoteTuner::getOriginRootNote() const
{
	return destinationRootNote;
}

double MidiNoteTuner::getOriginRootFreq() const
{
	return destinationRootFreq;
}

int MidiNoteTuner::getDestinationRootNote() const
{
    return destinationRootNote;
}

double MidiNoteTuner::getDestinationRootFreq() const
{
    return destinationRootFreq;
}

void MidiNoteTuner::setPitchbendRange(int pitchbendMaxIn)
{
    pitchbendRange = pitchbendMaxIn;
}

void MidiNoteTuner::setOriginRootNote(int rootNoteIn)
{
	originRootNote = rootNoteIn;
}

void MidiNoteTuner::setOriginRootFreq(double freqIn)
{
	originRootFreq = freqIn;
}

void MidiNoteTuner::setDestinationRootNote(int rootNoteIn)
{
	destinationRootNote = rootNoteIn;
}

void MidiNoteTuner::setDestinationRootFrequency(double freqIn)
{
    destinationRootFreq = freqIn;
}

void MidiNoteTuner::closestNote(int midiNoteIn, int& closestNoteOut, int& pitchbendOut)
{
	closestNoteOut = -1;

	double newCents = newTuning->getNoteInCents(midiNoteIn);
	int newMidiNote = originTuning->closestNoteToCents(newCents);
	double difference = newCents - originTuning->getNoteInSemitones(newMidiNote);

	if (abs(difference) <= pitchbendRange)
	{
		closestNoteOut = newMidiNote;
		pitchbendOut = semitonesToPitchbend(difference / 100.0);
	}
}

int MidiNoteTuner::pitchbendFromNote(int midiNoteIn) const
{
    return semitonesToPitchbend(pitchbendRange, semitonesFromNote(midiNoteIn));
} 

int MidiNoteTuner::pitchbendFromNote(int oldTuningNote, int newTuningNote) const
{
	return semitonesToPitchbend(pitchbendRange, newTuning->getNoteInSemitones(newTuningNote) - originTuning->getNoteInSemitones(oldTuningNote));
}

double MidiNoteTuner::semitonesFromNote(int midiNoteIn) const
{
	return newTuning->getNoteInSemitones(midiNoteIn) - originTuning->getNoteInSemitones(midiNoteIn);
}
int MidiNoteTuner::semitonesToPitchbend(double semitonesIn) const
{
	return semitonesToPitchbend(pitchbendRange, semitonesIn);
}

double MidiNoteTuner::pitchbendToSemitones(int pitchbendIn) const
{
	return pitchbendToSemitones(pitchbendRange, pitchbendIn);
}

int MidiNoteTuner::ratioToPitchbend(double ratioIn) const
{
    return semitonesToPitchbend(pitchbendRange, ratioToSemitones(ratioIn));
}

int MidiNoteTuner::pitchbendAmount(int pitchbendRange, double semitonesFrom, double semitonesTo)
{
	return semitonesToPitchbend(pitchbendRange, semitonesTo - semitonesFrom);
}

int MidiNoteTuner::semitonesToPitchbend(int pitchbendRange, double semitonesIn)
{
	return round((semitonesIn / pitchbendRange * 8192 + 8192));
}

double MidiNoteTuner::pitchbendToSemitones(int pitchbendRange, int pitchbendIn)
{
	return (pitchbendIn - 8192) / 16834.0 * pitchbendRange;
}

int MidiNoteTuner::ratioToPitchbend(int pitchbendRange, double ratioIn)
{
	return semitonesToPitchbend(pitchbendRange, ratioToSemitones(ratioIn));
}