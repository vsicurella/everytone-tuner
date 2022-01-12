/*
  ==============================================================================

    MidiNoteTuner.h
    Created: 30 Nov 2019 5:11:32pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "./tuning/MappedTuning.h"

struct MidiPitch
{
	int coarse = 0;
	int pitchbend = 8192;
	bool mapped = false;

	bool operator==(const MidiPitch& pitch)
	{
		return coarse == pitch.coarse 
			&& pitchbend == pitch.pitchbend 
			&& mapped == pitch.mapped;
	}

	// Comparisons always return true if callee is mapped and arg is unmapped

	bool isLowerThan(const MidiPitch& pitch) const
	{
		return (mapped && !pitch.mapped)
			|| (coarse < pitch.coarse 
				|| (coarse == pitch.coarse && pitchbend < pitch.pitchbend)
				);
	}

	bool isHigherThan(const MidiPitch& pitch) const
	{
		return (mapped && !pitch.mapped)
			|| (coarse > pitch.coarse
				|| (coarse == pitch.coarse && pitchbend > pitch.pitchbend)
				);
	}

	// -1 if this is lower than arg
	// 0 if they're the same
	// 1 if this is higher than arg
	int comparedTo(const MidiPitch& pitch) const
	{
		if (coarse == pitch.coarse)
		{
			if (pitchbend == pitch.pitchbend)
				return 0;

			if (pitchbend > pitch.pitchbend)
				return 1;

			if (pitchbend < pitch.pitchbend)
				return -1;
		}

		if (coarse < pitch.coarse)
			return -1;

		return 1;
	}
};

class MidiNoteTuner
{
	const std::shared_ptr<MappedTuningTable> sourceTuning;
	const std::shared_ptr<MappedTuningTable> targetTuning;

	int pitchbendRange; // total bipolar range of pitchbend in semitones

	bool cached = false; // TODO
	juce::Array<int> pitchbendTable;

public:
    
	MidiNoteTuner(std::shared_ptr<TuningTable> sourceTuning, 
				  std::shared_ptr<TuningTableMap> sourceMapping,
		          std::shared_ptr<TuningTable> targetTuning, 
		          std::shared_ptr<TuningTableMap> targetMapping,
				  int pitchbendRange = 4);
	MidiNoteTuner(const std::shared_ptr<MappedTuningTable>& mappedSource, const std::shared_ptr<MappedTuningTable>& mappedTarget, int pitchbendRange = 4);
    ~MidiNoteTuner();

	const TuningTable* tuningSource() const { return sourceTuning->getTuning(); }
	const TuningTable* tuningTarget() const { return targetTuning->getTuning(); }

	const TuningTableMap* mappingSource() const { return sourceTuning->getMapping(); }
	const TuningTableMap* mappingTarget() const { return targetTuning->getMapping(); }

	const MappedTuningTable* mappedSource() const { return sourceTuning.get(); }
	const MappedTuningTable* mappedTarget() const { return targetTuning.get(); }
    
    juce::Array<int> getPitchbendTable() const;

    int getPitchbendMax() const;
    
    void setPitchbendRange(int pitchBendMaxIn);

	//MappedNote getNoteMapping(int midiChannel, int midiNote) const;
	//MappedNote getNoteMapping(const juce::MidiMessage& msg) const;

	//MidiPitch getMidiPitch(const MappedNote& mappedNote) const;
	MidiPitch getMidiPitch(int midiChannel, int midiNote) const;
	MidiPitch getMidiPitch(const juce::MidiMessage& msg) const;


	/******************

		Member-oriented tuning functions

	******************/

	int semitonesToPitchbend(double semitonesIn) const;

	double pitchbendToSemitones(int pitchbendIn) const;
	
	int	ratioToPitchbend(double ratioIn) const;


	/******************

		Static tuning functions

	******************/

	static int pitchbendAmount(int pitchbendRange, double semitonesFrom, double semitonesTo);

	static int semitonesToPitchbend(int pitchbendRange, double semitonesIn);
	static double pitchbendToSemitones(int pitchbendRange, int pitchBendIn);

	static int ratioToPitchbend(int pitchbendRange, double ratioIn);

};
