/*
  ==============================================================================

    MidiNoteTuner.h
    Created: 30 Nov 2019 5:11:32pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
// #include "CommonFunctions.h"
#include "TuningMath.h"
#include "Tuning.h"
#include "TuningTableMap.h"
#include "MultichannelMap.h"

class MidiNoteTuner
{
	int pitchbendRange = 2; // total range of pitchbend in semitones

	// Source Tuning Parameters
	const Tuning* sourceTuning;

	// Target Tuning Parameters
	const Tuning* targetTuning;

	const Keytographer::TuningTableMap* tuningTableMap;


	bool cached = false;
	juce::Array<int> pitchbendTable;

	const Tuning standardTuning;
	const Keytographer::TuningTableMap standardMap = Keytographer::MultichannelMap::CreatePeriodicMapping(12, 60);

public:
    
    MidiNoteTuner(const Tuning* targetTuningIn = nullptr, const Keytographer::TuningTableMap* noteMapIn = nullptr);
	MidiNoteTuner(const Tuning* sourceTuningIn, const Tuning* targetTuningIn);
    ~MidiNoteTuner();

	const Tuning* getSourceTuning() { return sourceTuning; }
	const Tuning* getTargetTuning() { return targetTuning; }
    
    juce::Array<int> getPitchbendTable() const;

    int getPitchbendMax() const;

	void setSourceTuning(const Tuning* newSourceTuning);
	void setTargetTuning(const Tuning* newTuningIn);
    
    void setPitchbendRange(int pitchBendMaxIn);

	void setTuningTableMap(const Keytographer::TuningTableMap* mapIn);

	/// <summary>
	/// Use the current Tuning Table Map to change the MIDI note number
	/// and channel of the provided juce::MidiMessage.
	/// </summary>
	/// <param name="msg"></param>
	/// <returns>Semitone discrepancy between source and target note</returns>
	double mapMidiNote(juce::MidiMessage& msg);

	/// <summary>
	/// Remap the provided juce::MidiMessage via MidiNoteTuner::mapMidiNote
	/// and return the 14-bit pitchbend value to make up the source -> target discrepancy 
	/// </summary>
	/// <param name="msg"></param>
	/// <returns></returns>
	int mapNoteAndPitchbend(juce::MidiMessage& msg);


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
