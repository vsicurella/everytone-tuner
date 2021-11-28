/*
  ==============================================================================

    MidiNoteMapper.h
    Created: 19 Nov 2021 7:08:17pm
    Author:  vincenzo

    Wrapper for a TuningTableMap class - currently using MultichannelMap by default

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "MultichannelMap.h"

class MidiNoteMapper
{
    std::unique_ptr<Keytographer::TuningTableMap> map;

public:

    MidiNoteMapper();

    ~MidiNoteMapper();

    // void setMapping(const Keytographer::MultichannelMap& mapIn);

    Keytographer::MappedNote getNoteAt(int channel, int note) const;

};