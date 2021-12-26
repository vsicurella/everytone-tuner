/*
  ==============================================================================

    Map.h
    Created: 7 Nov 2021 10:34:41am
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#pragma once
#include "./TuningTableMap.h"

// Multichannel map
class MultichannelMap : public TuningTableMap
{
    // Maps per channel of MultichannelMap
    std::vector<Map<int>> maps;

    // The multimap index that considered the root map
    int multimapRootIndex = 0;

    // A lookup table by [midiChannel][midiNote]
    int** lookup;

public:

    struct Definition
    {
        TuningTableMap::Root root;
        std::vector<Map<int>> maps;
    };

private:

    static Map<int> buildMultimap(Definition definition);

public:

    MultichannelMap(Definition definition);

    MultichannelMap(const MultichannelMap&);

    virtual ~MultichannelMap();

    // void operator=(const MultichannelMap& mapToCopy);

    MultichannelMap::Definition getDefinition() const;

public:

    static TuningTableMap::Definition DefineTuningTableMap(Definition definition)
    {
        return TuningTableMap::Definition
        {
            TuningTableMap::Root { definition.root.midiChannel, definition.root.midiNote },
            buildMultimap(definition)
        };
    }

    static std::unique_ptr<TuningTableMap> CreateTuningTableMap(Definition definition)
    {
        auto d = DefineTuningTableMap(definition);
        return std::make_unique<TuningTableMap>(d);
    }

    /*
        Creates a MultichannelMap that have linear maps that are offset by a period from each other.
        The root MIDI Channel & Note marks 0 periods, and will output the rootTuningIndex parameter.
        The rootTuningIndex is automatically set if it's argument is out of bounds, which is default.
    */
    static TuningTableMap::Definition PeriodicMappingDefinition(int period, int rootMidiChannel, int rootMidiNote, int rootTuningIndex = -1)
    {
        // Multichannel MIDI Note index from 0 through 2047
        int rootMidiNoteIndex = (rootMidiChannel - 1) * 128 + rootMidiNote;

        int rootMidiChannelIndex = rootMidiChannel - 1;

        if (rootTuningIndex < 0 || rootTuningIndex >= 2048)
        {
            rootTuningIndex = rootMidiNoteIndex;
        }

        // The first tuning table index
        int currentTuningIndex = mod(rootTuningIndex - rootMidiNote - period * rootMidiChannelIndex, 2048);

        std::vector<Map<int>> maps;
        for (int m = 0; m < 16; m++)
        {
            Map<int>::FunctionDefinition fd =
            {
                128,
                currentTuningIndex,
                [=](int x) { return mod(x, 2048); }
            };

            maps.push_back(Map<int>(fd));

            currentTuningIndex = mod(currentTuningIndex + period, 2048);
        }

        MultichannelMap::Definition d =
        {
            TuningTableMap::Root { rootMidiChannel, rootMidiNote },
            maps                /* maps */
        };

        return DefineTuningTableMap(d);
    }
};
