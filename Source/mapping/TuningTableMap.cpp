/*
  ==============================================================================

    TuningTableMap.cpp
    Created: 26 Nov 2021 7:21:15pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TuningTableMap.h"

TuningTableMap::TuningTableMap(TuningTableMap::Definition definition)
    : rootMidiChannel(definition.root.midiChannel),
      rootMidiNote(definition.root.midiNote),
      map(std::make_unique<Map<int>>(definition.map)),
      transpose(definition.transpose)
{
    rebuildTable();
}

TuningTableMap::TuningTableMap(const TuningTableMap& mapToCopy)
    : rootMidiChannel(mapToCopy.rootMidiChannel),
      rootMidiNote(mapToCopy.rootMidiNote),
      map(new Map<int>(*mapToCopy.map.get())),
      transpose(mapToCopy.transpose)
{
    rebuildTable();
}

void TuningTableMap::operator=(const TuningTableMap& mapToCopy)
{
    rootMidiChannel = mapToCopy.rootMidiChannel;
    rootMidiNote = mapToCopy.rootMidiNote;
    map.reset(new Map<int>(mapToCopy.getDefinition().map));
    transpose = mapToCopy.transpose;
    rebuildTable();
}

void TuningTableMap::rebuildTable()
{

    for (int i = 0; i < 2048; i++)
    {
        int index = mod(i + transpose, 2048);
        table[i] = map->at(index);
    }
}

int TuningTableMap::period() const
{
    return map->base();
}

const Map<int>* TuningTableMap::midiIndexMap() const
{
    return map.get();
}

MappedNote TuningTableMap::getMappedNote(int midiNoteIndex) const
{
    int channel = midiNoteIndex / 128 + 1;
    int midiNote = midiNoteIndex % 128;

    bool mapped = midiNoteIndex >= 0 && midiNoteIndex < 2048;
   
    int fullIndex = (mapped) ? table[midiNoteIndex] : 0;
    int table = fullIndex / 128;
    int index = fullIndex % 128;

    MappedNote mapping =
    {
        channel,    /* input channel */
        midiNote,   /* input note */
        table,      /* table index 0-15 */
        index,      /* tuning index 0-127 */
        fullIndex,  /* tuning index 0-2047 */
        mapped,     /* If note is a valid mapping */
    };

    return mapping;
}

MappedNote TuningTableMap::getMappedNote(int channel, int midiNote) const
{
    return getMappedNote(midiIndex(channel, midiNote));
}

int TuningTableMap::getPatternIndex(int channel, int note)
{
    return map->mapIndexAt(midiIndex(channel, note));
}

//int TuningTableMap::periodsAt(int channel, int note)
//{
//    return map->basesAt(midiIndex(channel, note));
//}

int TuningTableMap::tableAt(int midiNoteIndex) const
{
    return table[midiNoteIndex];
}

TuningTableMap::Root TuningTableMap::getRoot() const
{
    return Root { rootMidiChannel, rootMidiNote };
}

int TuningTableMap::getTransposition() const
{
    return transpose;
}

void TuningTableMap::setTransposition(int transposeIn)
{
    transpose = transposeIn;
    rebuildTable();
}

std::shared_ptr<TuningTableMap> TuningTableMap::withTransposition(int transposeIn)
{
    auto definition = getDefinition();
    definition.transpose = transposeIn;
    return std::make_shared<TuningTableMap>(definition);
}

TuningTableMap::Definition TuningTableMap::getDefinition() const
{
    TuningTableMap::Definition definition =
    {
        TuningTableMap::Root { rootMidiChannel, rootMidiNote },
        *map,
        transpose
    };
    return definition;
}
