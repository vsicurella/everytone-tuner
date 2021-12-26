/*
  ==============================================================================

    Map.cpp
    Created: 7 Nov 2021 10:53:27am
    Author:  soundtoys

  ==============================================================================
*/

#include "MultichannelMap.h"

MultichannelMap::MultichannelMap(MultichannelMap::Definition definition)
    : TuningTableMap(initializePattern(definition)),
      numMaps(definition.numMaps),
      maps(definition.maps),
      rootMidiChannel(definition.rootMidiChannel) {}


MultichannelMap::MultichannelMap(const MultichannelMap& mapToCopy)
    : numMaps(mapToCopy.numMaps),
      maps(mapToCopy.maps),
      rootMidiChannel(mapToCopy.rootMidiChannel),
      TuningTableMap(initializePattern(mapToCopy.getDefinition())) {}

MultichannelMap::~MultichannelMap()
{
}

TuningTableMap::Definition MultichannelMap::initializePattern(MultichannelMap::Definition definition)
{
    auto multimap = buildMultimap(definition);

    TuningTableMap::Definition d =
    {
        definition.rootMidiChannel,
        definition.rootMidiNote,
        definition.rootTuningIndex,
        new Map<int>(multimap)
    };

    return d;
}

Map<int> MultichannelMap::buildMultimap(MultichannelMap::Definition definition)
{
    // Create multimap from maps
    int multipattern[2048];
    for (int m = 0; m < definition.numMaps; m++)
    {
        auto channelOffset = m * 128;
        auto map = &(definition.maps[m]);
        for (int i = 0; i < 128; i++)
        {
            auto index = channelOffset + i;
            multipattern[index] = map->at(i);
        }
    }

    // Probably should be reworked
    int period = definition.maps.at(definition.rootMidiChannel - 1).base();

    Map<int>::Definition d =
    {
        2048,           /* mapSize */
        multipattern,   /* pattern */
        period,         /* base */
        0,              /* pattern root */
        0,              /* map root*/
    };

    return Map<int>(d);
}

MultichannelMap::Definition MultichannelMap::getDefinition() const
{
    MultichannelMap::Definition definition = 
    {
        rootTuningIndex,
        rootMidiNote,
        rootMidiChannel,
        numMaps,
        maps,
    };

    return definition;
}