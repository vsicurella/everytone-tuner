/*
  ==============================================================================

    Map.cpp
    Created: 7 Nov 2021 10:53:27am
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "MultichannelMap.h"

MultichannelMap::MultichannelMap(MultichannelMap::Definition definition)
    : TuningTableMap(DefineTuningTableMap(definition)),
      maps(definition.maps) {}

MultichannelMap::MultichannelMap(const MultichannelMap& mapToCopy)
    : maps(mapToCopy.maps),
      TuningTableMap(DefineTuningTableMap(mapToCopy.getDefinition())) {}

MultichannelMap::~MultichannelMap() {}

Map<int> MultichannelMap::buildMultimap(MultichannelMap::Definition definition)
{
    // Create multimap from maps
    std::vector<int> multipattern;
    for (int m = 0; m < definition.maps.size(); m++)
    {
        auto channelOffset = m * 128;
        auto map = &(definition.maps[m]);
        for (int i = 0; i < 128; i++)
        {
            auto index = channelOffset + i;
            multipattern.push_back(map->at(i));
        }
    }

    // Probably should be reworked
    int period = definition.maps.at(definition.root.midiChannel - 1).base();

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
        TuningTableMap::Root { rootMidiChannel, rootMidiNote },
        maps,
    };

    return definition;
}