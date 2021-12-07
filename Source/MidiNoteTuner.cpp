/*
  ==============================================================================

    MidiNoteTuner.cpp
    Created: 30 Nov 2019 5:11:32pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "MidiNoteTuner.h"


MidiNoteTuner::MidiNoteTuner(const Tuning* targetTuningIn, const Keytographer::TuningTableMap* noteMapIn)
{
	sourceTuning = &standardTuning;

	targetTuning = (targetTuningIn == nullptr) ? &standardTuning : targetTuningIn;
	tuningTableMap = (noteMapIn == nullptr) ? &standardMap : noteMapIn;
}

MidiNoteTuner::MidiNoteTuner(const Tuning* sourceTuningIn, const Tuning* targetTuningIn)
	: sourceTuning(sourceTuningIn), targetTuning(targetTuningIn)
{
}

MidiNoteTuner::~MidiNoteTuner()
{
	tuningTableMap = nullptr;
}

void MidiNoteTuner::setSourceTuning(const Tuning* sourceTuningIn)
{
	sourceTuning = sourceTuningIn;
}

void MidiNoteTuner::setTargetTuning(const Tuning* newTuningIn)
{
	targetTuning = newTuningIn;
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

void MidiNoteTuner::setTuningTableMap(const Keytographer::TuningTableMap* mapIn)
{
	tuningTableMap = mapIn;
}

double MidiNoteTuner::mapMidiNote(juce::MidiMessage& msg)
{
	auto ch = msg.getChannel();
	auto note = msg.getNoteNumber();
	auto mapped = tuningTableMap->getMappedNote(ch, note);

	// First get target MTS note
	auto targetMts = targetTuning->mtsTableAt(mapped.index);

	// Then find closest source note
	auto sourceNote = sourceTuning->closestNoteIndex(targetMts);

	int newNote = sourceNote % 128;
	//int newChannel = (sourceNote / 128) + 1; // Unsure about channel re-mapping
	msg.setNoteNumber(newNote);

	auto sourceMts = sourceTuning->mtsTableAt(sourceNote);
	return targetMts - sourceMts;
}

int MidiNoteTuner::mapNoteAndPitchbend(juce::MidiMessage& msg)
{
	double discrepancy = mapMidiNote(msg);

	int pitchbend = 8192;
	if (abs(discrepancy) >= 1e-6)
	{
		pitchbend = semitonesToPitchbend(discrepancy);
		jassert(pitchbend >= 0 && pitchbend < (1 << 14));
	}

	return pitchbend;
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