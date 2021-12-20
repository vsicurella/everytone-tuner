/*
  ==============================================================================

    TuningHelpers.h
    Created: 11 Dec 2021 3:40:09pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Tuning.h"
#include "MultimapperCommon.h"

template <typename ARR>
static juce::ValueTree arrayToValueTree(ARR array, juce::Identifier id, juce::Identifier nodeId, juce::Identifier valueId = Multimapper::ID::Value)
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

static juce::ValueTree tuningToValueTree(const Tuning* tuning, juce::Identifier name = Multimapper::ID::Tuning)
{
    auto tree = juce::ValueTree(name);
    
    tree.setProperty(Multimapper::ID::Name, tuning->getName(), nullptr);
    tree.setProperty(Multimapper::ID::Description, tuning->getDescription(), nullptr);
    //tree.setProperty(Multimapper::ID::Transpose, tuning->tranpose)

    auto intervals = tuning->getIntervalCentsTable();
    auto intervalTree = arrayToValueTree(intervals, Multimapper::ID::IntervalTable, Multimapper::ID::Cents);
    tree.addChild(intervalTree, 0, nullptr);

    tree.setProperty(Multimapper::ID::RootMidiNote, tuning->getRootMidiNote(), nullptr);
    tree.setProperty(Multimapper::ID::RootMidiChannel, tuning->getRootMidiChannel(), nullptr);
    tree.setProperty(Multimapper::ID::RootFrequency, tuning->getRootFrequency(), nullptr);

    return tree;
}

static Tuning parseTuningValueTree(juce::ValueTree tree)
{
    auto definition = Tuning::CentsDefinition();
    
    juce::Array<double> cents;

    for (auto node : tree.getChild(0))
    {
        cents.add(node[Multimapper::ID::Value]);
    }

    definition.intervalCents = cents;

    definition.name =                       tree.getProperty(Multimapper::ID::Name, "");
    definition.description =                tree.getProperty(Multimapper::ID::Description, "");
    definition.transpose =                  tree.getProperty(Multimapper::ID::Transpose, 0);
    definition.reference.rootMidiNote =     tree.getProperty(Multimapper::ID::RootMidiNote, 60);
    definition.reference.rootMidiChannel =  tree.getProperty(Multimapper::ID::RootMidiChannel, 1);
    definition.reference.rootFrequency =    tree.getProperty(Multimapper::ID::RootFrequency, 256);

    return Tuning(definition);
}

static juce::ValueTree tuningTableMapToValueTree(const TuningTableMap& midiMap)
{
    auto tree = juce::ValueTree(Multimapper::ID::TuningTableMidiMap);
   
    auto definition = midiMap.getDefinition();

    tree.setProperty(Multimapper::ID::RootMidiNote,     definition.rootMidiNote,            nullptr);
    tree.setProperty(Multimapper::ID::RootTuningIndex,  definition.rootTuningIndex,         nullptr);

    tree.setProperty(Multimapper::ID::Period,           definition.map->base(),             nullptr);
    tree.setProperty(Multimapper::ID::PatternRoot,      definition.map->patternRoot(),      nullptr);
    tree.setProperty(Multimapper::ID::MapRoot,          definition.map->mapRoot(),          nullptr);
    tree.setProperty(Multimapper::ID::Transpose,        definition.map->transposition(),    nullptr);

    auto pattern = arrayToValueTree(definition.map->pattern(), Multimapper::ID::Pattern, Multimapper::ID::Pattern);

    tree.addChild(pattern, -1, nullptr);

    return tree;
}

static TuningTableMap parseTuningTableMapTree(juce::ValueTree tree)
{
    auto patternNode = tree.getChildWithName(Multimapper::ID::Pattern);
    const int mapSize = patternNode.getNumChildren();

    Map<int>::Pattern pattern;
    for (int i = 0; i < mapSize; i++)
    {
        pattern.push_back((int)patternNode.getChild(i)[Multimapper::ID::Value]);
    }

    int period = tree.getProperty(Multimapper::ID::Period, 0);
    int patternRoot = tree.getProperty(Multimapper::ID::PatternRoot, 0);
    int mapRoot = tree.getProperty(Multimapper::ID::MapRoot, 0);
    int transpose = tree.getProperty(Multimapper::ID::Transpose, 0);

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

    int rootMidiNote = tree.getProperty(Multimapper::ID::RootMidiNote);
    int rootTuningIndex = tree.getProperty(Multimapper::ID::RootTuningIndex);

    auto definition = TuningTableMap::Definition
    {
        rootMidiNote,
        rootTuningIndex,
        &map
    };

    auto tuningTableMap = TuningTableMap(definition);
    return tuningTableMap;
}