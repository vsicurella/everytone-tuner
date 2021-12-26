/*
  ==============================================================================

    MidiNoteTuner.cpp
    Created: 30 Nov 2019 5:11:32pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "MidiNoteTuner.h"


MidiNoteTuner::MidiNoteTuner(
	std::shared_ptr<Tuning> sourceTuningIn, 
	std::shared_ptr<TuningTableMap> sourceMappingIn,
	std::shared_ptr<Tuning> targetTuningIn, 
	std::shared_ptr<TuningTableMap> targetMappingIn, 
	int pitchbendRangeIn
)	: sourceTuning(std::make_unique<MappedTuning>(sourceTuningIn, sourceMappingIn)),
      targetTuning(std::make_unique<MappedTuning>(targetTuningIn, targetMappingIn)), 
      pitchbendRange(pitchbendRangeIn) {}

MidiNoteTuner::MidiNoteTuner(const std::shared_ptr<MappedTuning>& mappedSource, const std::shared_ptr<MappedTuning>& mappedTarget, int pitchbendRangeIn)
	: sourceTuning(mappedSource),
	  targetTuning(mappedTarget),
	  pitchbendRange(pitchbendRangeIn) {}

MidiNoteTuner::~MidiNoteTuner() {}

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

//MappedNote MidiNoteTuner::getNoteMapping(int midiChannel, int midiNote) const
//{
//	return tuningTableMap->getMappedNote(midiChannel, midiNote);
//}
//
//MappedNote MidiNoteTuner::getNoteMapping(const juce::MidiMessage& msg) const
//{
//	return getNoteMapping(msg.getChannel(), msg.getNoteNumber());
//}

//MidiPitch MidiNoteTuner::getMidiPitch(const MappedNote& mapped) const
//{
//	// First get target MTS note
//	auto targetMts = targetTuning->mtsTableAt(mapped.index);
//
//	if (targetMts < 0 || targetMts >= 128)
//		return MidiPitch();
//
//	// Then find closest source note
//	auto sourceNote = sourceTuning->closestNoteIndex(targetMts);
//
//	if (sourceNote < 0 || sourceNote >= 128)
//		return MidiPitch();
//
//	// Last, find discrepancy and convert to pitchbend
//	auto sourceMts = sourceTuning->mtsTableAt(sourceNote);
//
//	if (sourceMts < 0 || sourceMts > 127)
//		return MidiPitch();
//
//	double discrepancy = targetMts - sourceMts;
//
//	int pitchbend = 8192;
//	if (abs(discrepancy) >= 1e-6)
//	{
//		pitchbend = semitonesToPitchbend(discrepancy);
//		jassert(pitchbend >= 0 && pitchbend < (1 << 14));
//	}
//
//	MidiPitch pitch = { sourceNote, pitchbend, true };
//	return pitch;
//}

MidiPitch MidiNoteTuner::getMidiPitch(int midiChannel, int midiNote) const
{
	// First get target MTS note
	auto targetMts = targetTuning->mtsAt(midiNote, midiChannel);
	
	if (targetMts < 0 || targetMts >= 128)
		return MidiPitch();
	
	// Then find closest source note
	auto sourceIndex = sourceTuning->closestIndexToMts(targetMts);
	auto sourceMts = sourceTuning->mtsAt(sourceIndex);

	//if (sourceNote < 0 || sourceNote >= 128)
	//	return MidiPitch();
	//
	//auto sourceMts = sourceTuning->mtsTableAt(sourceNote);
	// Last, find discrepancy and convert to pitchbend
	
	if (sourceMts < 0 || sourceMts > 127)
		return MidiPitch();
	
	double discrepancy = targetMts - sourceMts;
	
	int pitchbend = 8192;
	if (abs(discrepancy) >= 1e-6)
	{
		pitchbend = semitonesToPitchbend(discrepancy);
		jassert(pitchbend >= 0 && pitchbend < (1 << 14));
	}
	
	// TODO sourceIndex may not be desired for non-standard source tunings
	MidiPitch pitch = { sourceIndex, pitchbend, true };
	return pitch;
}


MidiPitch MidiNoteTuner::getMidiPitch(const juce::MidiMessage& msg) const
{
	auto ch = msg.getChannel();
	auto note = msg.getNoteNumber();
	return getMidiPitch(ch, note);
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
	return round((semitonesIn / pitchbendRange * 16384.0 + 8192));
}

double MidiNoteTuner::pitchbendToSemitones(int pitchbendRange, int pitchbendIn)
{
	return (pitchbendIn - 8192) / 16834.0 * pitchbendRange;
}

int MidiNoteTuner::ratioToPitchbend(int pitchbendRange, double ratioIn)
{
	return semitonesToPitchbend(pitchbendRange, ratioToSemitones(ratioIn));
}