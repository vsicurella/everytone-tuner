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
};

class MidiNoteTuner
{
	const std::shared_ptr<MappedTuning> sourceTuning;
	const std::shared_ptr<MappedTuning> targetTuning;

	int pitchbendRange; // total bipolar range of pitchbend in semitones

	bool cached = false; // TODO
	juce::Array<int> pitchbendTable;

public:
    
	MidiNoteTuner(std::shared_ptr<Tuning> sourceTuning, 
				  std::shared_ptr<TuningTableMap> sourceMapping,
		          std::shared_ptr<Tuning> targetTuning, 
		          std::shared_ptr<TuningTableMap> targetMapping,
				  int pitchbendRange = 4);
	MidiNoteTuner(const std::shared_ptr<MappedTuning>& mappedSource, const std::shared_ptr<MappedTuning>& mappedTarget, int pitchbendRange = 4);
    ~MidiNoteTuner();

	const Tuning* tuningSource() const { return sourceTuning->getTuning(); }
	const Tuning* tuningTarget() const { return targetTuning->getTuning(); }

	const TuningTableMap* mappingSource() const { return sourceTuning->getMapping(); }
	const TuningTableMap* mappingTarget() const { return targetTuning->getMapping(); }

	const MappedTuning* mappedSource() const { return sourceTuning.get(); }
	const MappedTuning* mappedTarget() const { return targetTuning.get(); }
    
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
