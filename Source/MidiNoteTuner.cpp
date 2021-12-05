/*
  ==============================================================================

    MidiNoteTuner.cpp
    Created: 30 Nov 2019 5:11:32pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "MidiNoteTuner.h"


MidiNoteTuner::MidiNoteTuner(const Tuning* targetTuningIn)
{
	sourceTuning = &standard;

	if (targetTuningIn == nullptr)
		targetTuning = &standard;
	else
		targetTuning = targetTuningIn;

	tuningTableMap = std::make_unique<Keytographer::TuningTableMap>(Keytographer::MultichannelMap::CreatePeriodicMapping(12, 60));
}

MidiNoteTuner::MidiNoteTuner(const Tuning* sourceTuningIn, const Tuning* targetTuningIn)
	: sourceTuning(sourceTuningIn), targetTuning(targetTuningIn)
{
	tuningTableMap = std::make_unique<Keytographer::TuningTableMap>(Keytographer::MultichannelMap::CreatePeriodicMapping(12, 60));
}

MidiNoteTuner::~MidiNoteTuner()
{
	tuningTableMap = nullptr;
}

void MidiNoteTuner::setSourceTuning(const Tuning& sourceTuningIn)
{
	sourceTuning = &sourceTuningIn;
}

void MidiNoteTuner::setTargetTuning(const Tuning& newTuningIn)
{
	targetTuning = &newTuningIn;
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

void MidiNoteTuner::setTuningTableMap(Keytographer::TuningTableMap* mapIn)
{
	tuningTableMap.reset(new Keytographer::TuningTableMap(*mapIn));
}

int MidiNoteTuner::tuneNoteAndGetPitchbend(juce::MidiMessage& msg)
{
	auto ch = msg.getChannel();
	auto note = msg.getNoteNumber();
	auto isnoteon = msg.isNoteOn();

	juce::String dbgmsg = "In. (Ch: " + juce::String(ch) + ", N: " + juce::String(note);

	auto mapped = tuningTableMap->getMappedNote(ch, note);
	
	// First get target MTS note
	auto targetMts = targetTuning->mtsTableAt(mapped.index);

	dbgmsg += ") -> tI: " + juce::String(mapped.index) + " (" + juce::String(roundN(3, targetMts));

	// Then find closest source note
	auto sourceNote = sourceTuning->closestNoteIndex(targetMts);

	int newNote = sourceNote % 128;
	int newChannel = (sourceNote / 128) + 1; // Unsure about channel re-mapping
	msg.setNoteNumber(newNote);
	msg.setChannel(newChannel);

	// Lastly find pitchbend amount
	auto sourceMts = sourceTuning->mtsTableAt(sourceNote);
	double difference = targetMts - sourceMts;

	dbgmsg += ") from sI: " + juce::String(sourceNote) + " (" + juce::String(roundN(3, sourceMts)) + ")";
	if (msg.isNoteOn())
		juce::Logger::writeToLog(dbgmsg);

	int pitchbendOut = 8192;
	if (abs(difference) <= pitchbendRange)
	{
		pitchbendOut = semitonesToPitchbend(difference / 100.0);
	}

	return pitchbendOut;
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