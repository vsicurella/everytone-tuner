/*
  ==============================================================================

    MidiNoteMapper.cpp
    Created: 19 Nov 2021 7:08:17pm
    Author:  vincenzo

  ==============================================================================
*/

#include "MidiNoteMapper.h"

MidiNoteMapper::MidiNoteMapper()
{
    map.reset(new Keytographer::MultichannelMap(Keytographer::StandardMapping()));
}

MidiNoteMapper::~MidiNoteMapper()
{

}

// void MidiNoteMapper::setMapping(const Keytographer::TuningTableMap& mapIn)
// {
//     map = std::make_unique<Keytographer::TuningTableMap>(mapIn);
// }

Keytographer::MappedNote MidiNoteMapper::getNoteAt(int channel, int note) const
{
    return map->getMappedNote(channel, note);
}