/*
  ==============================================================================

    MidiNoteTuner.cpp
    Created: 30 Nov 2019 5:11:32pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "MidiNoteTuner.h"


MidiNoteTuner::MidiNoteTuner(std::shared_ptr<Tuning> sourceTuningIn, std::shared_ptr<Tuning> targetTuningIn, std::shared_ptr<Keytographer::TuningTableMap> mapping)
	: sourceTuning(sourceTuningIn), targetTuning(targetTuningIn), tuningTableMap(mapping)
{
}

MidiNoteTuner::~MidiNoteTuner()
{
}

juce::Array<int> MidiNoteTuner::getPitchbendTable() const
{
	return pitchbendTable;
}

int MidiNoteTuner::getPitchbendMax() const
{
    return pitchbendRange;
}

void MidiNoteTuner::setPitchbendRange(int pitchbendMaxIn)
{
    pitchbendRange = pitchbendMaxIn;
}

Keytographer::MappedNote MidiNoteTuner::getNoteMapping(int midiChannel, int midiNote) const
{
	return tuningTableMap->getMappedNote(midiChannel, midiNote);
}

Keytographer::MappedNote MidiNoteTuner::getNoteMapping(const juce::MidiMessage& msg) const
{
	return getNoteMapping(msg.getChannel(), msg.getNoteNumber());
}

MidiPitch MidiNoteTuner::getMidiPitch(const Keytographer::MappedNote& mapped) const
{
	// First get target MTS note
	auto targetMts = targetTuning->mtsTableAt(mapped.index);

	if (targetMts < 0 || targetMts > 127)
		return MidiPitch();

	// Then find closest source note
	auto sourceNote = sourceTuning->closestNoteIndex(targetMts);

	// Last, find discrepancy and convert to pitchbend
	int newNote = sourceNote % 128;
	auto sourceMts = sourceTuning->mtsTableAt(sourceNote);

	if (sourceMts < 0 || sourceMts > 127)
		return MidiPitch();

	double discrepancy = targetMts - sourceMts;

	int pitchbend = 8192;
	if (abs(discrepancy) >= 1e-6)
	{
		pitchbend = semitonesToPitchbend(discrepancy);
		jassert(pitchbend >= 0 && pitchbend < (1 << 14));
	}

	MidiPitch pitch = { sourceNote, pitchbend, true };
	return pitch;
}

MidiPitch MidiNoteTuner::getMidiPitch(const juce::MidiMessage& msg) const
{
	auto ch = msg.getChannel();
	auto note = msg.getNoteNumber();
	return getMidiPitch(ch, note);
}

MidiPitch MidiNoteTuner::getMidiPitch(int midiChannel, int midiNote) const
{
	auto mapped = getNoteMapping(midiChannel, midiNote);
	return getMidiPitch(mapped);
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