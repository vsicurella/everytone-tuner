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
    
    tree.setProperty(Everytone::ID::Name, tuning->getName(), nullptr);
    tree.setProperty(Everytone::ID::Description, tuning->getDescription(), nullptr);

    auto intervals = tuning->getIntervalCentsTable();
    auto intervalTree = arrayToValueTree(intervals, Everytone::ID::IntervalTable, Everytone::ID::Cents);
    tree.addChild(intervalTree, 0, nullptr);

    tree.setProperty(Everytone::ID::RootFrequency, tuning->getRootFrequency(), nullptr);

    return tree;
}

static Tuning parseTuningValueTree(juce::ValueTree tree)
{
    auto definition = CentsDefinition();
    
    juce::Array<double> cents;

    for (auto node : tree.getChild(0))
    {
        cents.add(node[Everytone::ID::Value]);
    }

    definition.intervalCents = cents;

    definition.name =                       tree.getProperty(Everytone::ID::Name, "");
    definition.description =                tree.getProperty(Everytone::ID::Description, "");
    definition.rootFrequency =    tree.getProperty(Everytone::ID::RootFrequency, 440);

    return Tuning(definition);
}

static juce::ValueTree tuningTableMapToValueTree(const TuningTableMap& midiMap)
{
    auto tree = juce::ValueTree(Everytone::ID::TuningTableMidiMap);
   
    auto definition = midiMap.getDefinition();

    tree.setProperty(Everytone::ID::RootMidiNote,     definition.rootMidiNote,            nullptr);
    tree.setProperty(Everytone::ID::RootTuningIndex,  definition.rootTuningIndex,         nullptr);

    tree.setProperty(Everytone::ID::Period,           definition.map->base(),             nullptr);
    tree.setProperty(Everytone::ID::PatternRoot,      definition.map->patternRoot(),      nullptr);
    tree.setProperty(Everytone::ID::MapRoot,          definition.map->mapRoot(),          nullptr);
    tree.setProperty(Everytone::ID::Transpose,        definition.map->transposition(),    nullptr);

    auto pattern = arrayToValueTree(definition.map->pattern(), Everytone::ID::Pattern, Everytone::ID::Pattern);

    tree.addChild(pattern, -1, nullptr);

    return tree;
}

static TuningTableMap parseTuningTableMapTree(juce::ValueTree tree)
{
    auto patternNode = tree.getChildWithName(Everytone::ID::Pattern);
    const int mapSize = patternNode.getNumChildren();

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

    auto map = Map<int>(mapDefinition);

    int rootMidiNote = tree.getProperty(Everytone::ID::RootMidiNote);
    int rootTuningIndex = tree.getProperty(Everytone::ID::RootTuningIndex);

    auto definition = TuningTableMap::Definition
    {
        rootMidiNote,
        rootTuningIndex,
        &map
    };

    auto tuningTableMap = TuningTableMap(definition);
    return tuningTableMap;
}