/*
  ==============================================================================

    Mapping.h
    Created: 4 Nov 2021 12:08:30pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#pragma once

#include "./Map.h"

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

    // Constructor parameters for TuningTableMap
    // input: rootMidiNote, output: rootTuningIndex
    struct Definition
    {
        int rootMidiNote = 60;
        int rootTuningIndex = 60;
        Map<int>* map;
    };

protected:
        
    // MIDI Note to align with tuning table root
    int rootMidiNote;

    // Tuning table index to align with map root
    int rootTuningIndex;

    // MIDI Note to Tuning Table index map
    std::unique_ptr<Map<int>> map;

    // Cached map for Multichannel MIDI range
    int table[2048];

private:

    int midiIndex(int midiChannel, int midiNote) const;

    void rebuildTable();

public:

    TuningTableMap(Definition definition);

    TuningTableMap(const TuningTableMap& mapToCopy);

    virtual ~TuningTableMap() {}

    void operator=(const TuningTableMap& mapToCopy);

    int period() const;

    const Map<int>* midiIndexMap() const;

    MappedNote getMappedNote(int midiNoteIndex) const;

    MappedNote getMappedNote(int channel, int note) const;

    int getPatternIndex(int channel, int note);

    int tableAt(int midiNoteIndex) const;

    //int periodsAt(int channel, int note);

    TuningTableMap::Definition getDefinition() const;
};

/// <summary>
/// Returns a 1:1 MIDI note to tuning table index mapping that repeats every 128 notes
/// </summary>
/// <returns></returns>
static TuningTableMap StandardMapping()
{
    Map<int>::FunctionDefinition mapDefinition =
    {
        128,
        0,
        [&](int x) { return x % 128; },
        0,
        0
    };

    auto map = Map<int>(mapDefinition);
        
    TuningTableMap::Definition definition;
    definition.map = &map;

    auto tuningTableMap = TuningTableMap(definition);
    return tuningTableMap;
}
