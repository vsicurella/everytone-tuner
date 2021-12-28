/*
  ==============================================================================

    TuningHelpers.h
    Created: 11 Dec 2021 3:40:09pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "./tuning/Tuning.h"
#include "Common.h"

template <typename ARR>
static juce::ValueTree arrayToValueTree(ARR array, juce::Identifier id, juce::Identifier nodeId, juce::Identifier valueId = Everytone::ID::Value)
{
    auto tree = juce::ValueTree(id);
    for (auto value : array)
    {
        auto node = juce::ValueTree(nodeId);
        node.setProperty(valueId, value, nullptr);
        tree.addChild(node, -1, nullptr);
    }

    return tree;
}

static juce::ValueTree tuningToValueTree(const Tuning* tuning, juce::Identifier name = Everytone::ID::Tuning)
{
    auto tree = juce::ValueTree(name);
    
    auto intervals = tuning->getIntervalCentsTable();
    auto intervalTree = arrayToValueTree(intervals, Everytone::ID::IntervalTable, Everytone::ID::Cents);
    tree.addChild(intervalTree, 0, nullptr);

    tree.setProperty(Everytone::ID::RootFrequency,  tuning->getRootFrequency(), nullptr);
    tree.setProperty(Everytone::ID::Name,           tuning->getName(), nullptr);
    tree.setProperty(Everytone::ID::Description,    tuning->getDescription(), nullptr);
    tree.setProperty(Everytone::ID::VirtualPeriod,  tuning->getVirtualPeriod(), nullptr);
    tree.setProperty(Everytone::ID::VirtualSize,    tuning->getVirtualSize(), nullptr);
    
    return tree;
}

static CentsDefinition parseTuningValueTree(juce::ValueTree tree)
{
    juce::Array<double> cents;

    for (auto node : tree.getChild(0))
    {
        cents.add(node[Everytone::ID::Value]);
    }

    if (cents.size() == 0)
    {
        return CentsDefinition();
    }

    CentsDefinition definition =
    {
        cents,
        (double)tree.getProperty(Everytone::ID::RootFrequency, 440.0),
        tree.getProperty(Everytone::ID::Name, "").toString(),
        tree.getProperty(Everytone::ID::Description, "").toString(),
        (double)tree.getProperty(Everytone::ID::VirtualPeriod, 2.0),
        (double)tree.getProperty(Everytone::ID::VirtualSize, 12.0)
    };
    
    return definition;
}

static juce::ValueTree tuningTableMapToValueTree(const TuningTableMap* midiMap, juce::Identifier name = Everytone::ID::TuningTableMidiMap)
{
    auto tree = juce::ValueTree(name);
    auto definition = midiMap->getDefinition();
    
    tree.setProperty(Everytone::ID::RootMidiChannel,  definition.root.midiChannel,      nullptr);
    tree.setProperty(Everytone::ID::RootMidiNote,     definition.root.midiNote,         nullptr);
    tree.setProperty(Everytone::ID::Period,           definition.map.base(),           nullptr);
    tree.setProperty(Everytone::ID::PatternRoot,      definition.map.patternRoot(),    nullptr);
    tree.setProperty(Everytone::ID::MapRoot,          definition.map.mapRoot(),        nullptr);
    tree.setProperty(Everytone::ID::Transpose,        definition.map.transposition(),  nullptr);

    auto pattern = arrayToValueTree(definition.map.pattern(), Everytone::ID::Pattern, Everytone::ID::Pattern);

    tree.addChild(pattern, -1, nullptr);

    return tree;
}

static TuningTableMap::Definition parseTuningTableMapTree(juce::ValueTree tree)
{
    auto patternNode = tree.getChildWithName(Everytone::ID::Pattern);
    const int mapSize = patternNode.getNumChildren();

    if (mapSize == 0)
        return TuningTableMap::StandardMappingDefinition();

    Map<int>::Pattern pattern;
    for (int i = 0; i < mapSize; i++)
    {
        pattern.push_back((int)patternNode.getChild(i)[Everytone::ID::Value]);
    }

    int period = tree.getProperty(Everytone::ID::Period, 0);
    int patternRoot = tree.getProperty(Everytone::ID::PatternRoot, 0);
    int mapRoot = tree.getProperty(Everytone::ID::MapRoot, 0);
    int transpose = tree.getProperty(Everytone::ID::Transpose, 0);

    auto mapDefinition = Map<int>::Definition
    {
        mapSize,
        pattern.data(),
        period,
        patternRoot,
        mapRoot,
        transpose
    };

    TuningTableMap::Definition definition
    {
        TuningTableMap::Root 
        {
            (int)tree.getProperty(Everytone::ID::RootMidiChannel),
            (int)tree.getProperty(Everytone::ID::RootMidiNote)
        },

        Map<int>(mapDefinition)
    };

    return definition;
}

static juce::ValueTree mappedTuningToValueTree(const MappedTuning* mappedTuning, juce::Identifier name)
{
    auto tuningTree = tuningToValueTree(mappedTuning->getTuning());
    auto mappingTree = tuningTableMapToValueTree(mappedTuning->getMapping());

    auto tree = juce::ValueTree(name);
    tree.addChild(tuningTree, 0, nullptr);
    tree.addChild(mappingTree, 1, nullptr);
    return tree;
}