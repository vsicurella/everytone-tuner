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

class MidiNoteTuner
{
	int pitchbendRange = 2; // total range of pitchbend in semitones

	// Origin Tuning Parameters
	int originRootNote = 69;
	double originRootFreq = 440;
	Tuning const* originTuning;

	// Destination Tuning Parameters
	int destinationRootNote = 69;
	double destinationRootFreq = 440;
	Tuning const* newTuning;

	bool cached = false;
    juce::Array<int> pitchbendTable;

	Tuning* standard;

public:
    
	MidiNoteTuner(const Tuning& defaultTuning, const Tuning& newTuning);
    //MidiNoteTuner(const Tuning* tuningToUse=nullptr);
    ~MidiNoteTuner();

	void setOriginTuning(const Tuning& newOriginTuning);
    
    void setNewTuning(const Tuning& newTuningIn);
    
    juce::Array<int> getPitchbendTable() const;

    int getPitchbendMax() const;
    
    int getOriginRootNote() const;
	double getOriginRootFreq() const;
    
    int getDestinationRootNote() const;
    double getDestinationRootFreq() const;
    
    void setPitchbendRange(int pitchBendMaxIn);

	void setOriginRootNote(int rootNoteIn);
	void setOriginRootFreq(double freqIn);
    
    void setDestinationRootNote(int rootNoteIn);
    void setDestinationRootFrequency(double freqIn);

	/******************

		Member-oriented tuning functions

	******************/

	int semitonesToPitchbend(double semitonesIn) const;

	double pitchbendToSemitones(int pitchbendIn) const;
	
	int	ratioToPitchbend(double ratioIn) const;

    /*
        Returns a note struct with the note number that needs the least amount of pitchbend to sound like the destination note
    */
    void closestNote(int midiNoteIn, int& closestNoteOut, int& pitchbendOut);

	/*
		Returns the difference between input note and destination note in semitones
	*/
	double semitonesFromNote(int midiNoteIn) const;
    
	/*
		Returns the total amount of pitchbend needed to make the input midi note sound like the destination note.
		Be careful, as this can be beyond the range of valid pitchbend (< 0 or >= 16384)
	*/

	int pitchbendFromNote(int midiNoteIn) const;

	/*
		Returns the total amount of pitchbend to get from one note in the original tuning
		to a different note in the new tuning
	*/
	int pitchbendFromNote(int oldTuningNote, int newTuningNote) const;


	/******************

		Static tuning functions

	******************/

	static int pitchbendAmount(int pitchbendRange, double semitonesFrom, double semitonesTo);

	static int semitonesToPitchbend(int pitchbendRange, double semitonesIn);
	static double pitchbendToSemitones(int pitchbendRange, int pitchBendIn);

	static int ratioToPitchbend(int pitchbendRange, double ratioIn);

};
