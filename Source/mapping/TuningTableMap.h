/*
  ==============================================================================

    Mapping.h
    Created: 4 Nov 2021 12:08:30pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#pragma once

#include "./Map.h"

static int midiIndex(int midiChannel, int midiNote)
{
    return (midiChannel - 1) * 128 + midiNote;
}

struct MappedNote
{
    int inputChannel = 0;  // MIDI input channel
    int inputNote = 0;     // MIDI input note

    int tableIndex = 0;    // 0 through 15
    int noteIndex = 0;     // 0 through 127

    int index = 0;         // 0 through 2047

    bool mapped = false;   // False if note should be ignored
};

class TuningTableMap
{
public:

    struct Root
    {
        int midiChannel = 1;
        int midiNote = 60;

        bool operator==(const TuningTableMap::Root& root) const
        {
            return midiChannel == root.midiChannel && midiNote == root.midiNote;
        }

        bool operator!=(const TuningTableMap::Root& root) const { return operator==(root); }
    };

    // Constructor parameters for TuningTableMap
    // input: rootMidiNote, output: rootTuningIndex
    struct Definition
    {
        Root root;
        Map<int> map;
        int transpose = 0;
    };

protected:

    // MIDI Channel and Note combine to set the mapping root
    // Metadata from the map construction
    int rootMidiChannel;
    int rootMidiNote;

    // MIDI Note to Tuning Table index map
    std::unique_ptr<Map<int>> map;

    // Number of steps to offset the output by, 
    // best used when Tuning Reference and Mapping Root aren't the same
    int transpose = 0;

    // Cached map for Multichannel MIDI range
    int table[2048];

private:

    void rebuildTable();

public:

    TuningTableMap(Definition definition);

    TuningTableMap(const TuningTableMap& mapToCopy);

    virtual ~TuningTableMap() {}

    void operator=(const TuningTableMap& mapToCopy);

    int getRootMidiChannel() const { return rootMidiChannel; }
    int getRootMidiNote() const { return rootMidiNote; }
    int getRootMidiIndex() const { return midiIndex(rootMidiChannel, rootMidiNote); }

    Root getRoot() const;

    int period() const;

    const Map<int>* midiIndexMap() const;

    MappedNote getMappedNote(int midiNoteIndex) const;

    MappedNote getMappedNote(int channel, int note) const;

    //int periodsAt(int channel, int note);


    int getTransposition() const;
    void setTransposition(int transposeIn);
    std::shared_ptr<TuningTableMap> withTransposition(int transposeIn);

    Definition getDefinition() const;

    int getPatternIndex(int channel, int note);

    int tableAt(int midiNoteIndex) const;



public:

    /// <summary>
    /// Returns a 1:1 MIDI note to tuning table index mapping that repeats every 128 notes
    /// </summary>
    /// <returns></returns>
    static TuningTableMap::Definition StandardMappingDefinition()
    {
        Map<int>::FunctionDefinition mapDefinition =
        {
            128,
            0,
            [&](int x) { return x % 128; },
            0,
            0
        };

        return TuningTableMap::Definition
        {
            TuningTableMap::Root { 1, 69 },
            Map<int>(mapDefinition)
        };
    }

    /*
    Creates a MultichannelMap that is linear, with a wraparound wherever note 2048 lands
    The root MIDI Channel & Note marks 0 periods, and will output the rootTuningIndex parameter.
    The rootTuningIndex is automatically set if it's argument is out of bounds, which is default.
    */
    static TuningTableMap::Definition LinearMappingDefinition(int rootMidiChannel, int rootMidiNote, int tuningRootIndex, int tuningTableSize)
    {
        int midiIndex = mod((rootMidiChannel - 1) * 128 + rootMidiNote, 2048);

        auto mapFunction = Map<int>::FunctionDefinition
        {
            2048,
            0,
            [&](int x) { return mod(x - midiIndex + tuningRootIndex, tuningTableSize); },
            0
        };

        return TuningTableMap::Definition
        {
            TuningTableMap::Root { rootMidiChannel, rootMidiNote },
            Map<int>(mapFunction)
        };
    }
};
