/*
  ==============================================================================

    TuningHelpers.h
    Created: 11 Dec 2021 3:40:09pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "./tuning/MappedTuning.h"
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

static juce::ValueTree tuningBaseToValueTree(const TuningBase* tuning, juce::ValueTree parent = juce::ValueTree())
{
    auto tree = parent;
    if (!parent.isValid())
        tree = juce::ValueTree(Everytone::ID::Tuning);
    
    tree.setProperty(Everytone::ID::RootFrequency,      tuning->getRootFrequency(), nullptr);
    tree.setProperty(Everytone::ID::RootTuningIndex,    tuning->getRootIndex(), nullptr);
    tree.setProperty(Everytone::ID::Name,               tuning->getName(), nullptr);
    tree.setProperty(Everytone::ID::Description,        tuning->getDescription(), nullptr);
    
    return tree;
}

static juce::ValueTree tuningTableToValueTree(const TuningTable* tuning, juce::ValueTree parent = juce::ValueTree())
{
    auto tree = parent;
    if (!parent.isValid())
        tree = juce::ValueTree(Everytone::ID::Tuning);

    tuningBaseToValueTree(tuning, tree);

    tree.setProperty(Everytone::ID::PeriodString, tuning->getPeriodString(), nullptr);
    tree.setProperty(Everytone::ID::VirtualPeriod, tuning->getVirtualPeriod(), nullptr);
    tree.setProperty(Everytone::ID::VirtualSize, tuning->getVirtualSize(), nullptr);

    auto frequencies = tuning->getFrequencyTable();
    auto frequencyNode = arrayToValueTree(frequencies, Everytone::ID::FrequencyTable, Everytone::ID::Frequency);
    tree.addChild(frequencyNode, 0, nullptr);

    return tree;
}

static juce::ValueTree functionalTuningToValueTree(const FunctionalTuning* tuning, juce::ValueTree parent = juce::ValueTree())
{
    auto tree = parent;
    if (!parent.isValid())
        tree = juce::ValueTree(Everytone::ID::Tuning);

    tuningTableToValueTree(tuning, tree);

    auto intervals = tuning->getIntervalCentsList();
    auto intervalTree = arrayToValueTree(intervals, Everytone::ID::CentsTable, Everytone::ID::Cents);
    tree.addChild(intervalTree, 0, nullptr);

    return tree;
}

static juce::ValueTree tuningToValueTree(const TuningBase* tuning, juce::Identifier name = Everytone::ID::Tuning)
{
    auto tree = juce::ValueTree(name);

    auto functional = dynamic_cast<const FunctionalTuning*>(tuning);
    if (functional != nullptr)
        return functionalTuningToValueTree(functional, tree);

    auto table = dynamic_cast<const TuningTable*>(tuning);
    if (table != nullptr)
        return tuningTableToValueTree(table, tree);

    return tuningBaseToValueTree(tuning, tree);
}

static void applyTuningBasePropertiesFromValueTree(juce::ValueTree tree, int& rootIndex, double& rootFrequency, juce::String& name, juce::String& description)
{
    rootIndex       = (int)tree[Everytone::ID::RootTuningIndex];
    rootFrequency   = (double)tree[Everytone::ID::RootFrequency];
    name            = tree[Everytone::ID::Name].toString();
    description     = tree[Everytone::ID::Description].toString();
}

static void applyTuningTablePropertiesFromValueTree(
    juce::ValueTree tree,
    int& rootIndex, 
    juce::String& name, 
    juce::String& description,
    juce::String& periodString, 
    double& virtualPeriod,
    double& virtualSize)
{
    periodString = tree[Everytone::ID::PeriodString].toString();
    virtualPeriod = (double)tree[Everytone::ID::VirtualPeriod];
    virtualSize = (double)tree[Everytone::ID::VirtualSize];

    double dummyFreq;
    applyTuningBasePropertiesFromValueTree(tree, rootIndex, dummyFreq, name, description);
}


static std::shared_ptr<FunctionalTuning> constructFunctionalTuningFromValueTree(juce::ValueTree tree)
{
    auto centsNode = tree.getChildWithName(Everytone::ID::CentsTable);
    if (!centsNode.isValid())
        return nullptr;

    juce::Array<double> cents;
    for (auto child : centsNode)
    {
        auto value = (double)child[Everytone::ID::Value];
        cents.add(value);
    }

    if (cents.size() == 0)
        return nullptr;

    CentsDefinition definition { cents };
    
    definition.rootFrequency = (double)tree[Everytone::ID::RootFrequency];
    definition.name = tree[Everytone::ID::Name].toString();
    definition.description= tree[Everytone::ID::Description].toString();
    definition.virtualPeriod = (double)tree[Everytone::ID::VirtualPeriod];
    definition.virtualSize = (double)tree[Everytone::ID::VirtualSize];

    return std::make_shared<FunctionalTuning>(definition);
}

static std::shared_ptr<TuningTable> constructTuningTableFromValueTree(juce::ValueTree tree)
{
    auto frequencyNode = tree.getChildWithName(Everytone::ID::FrequencyTable);
    if (!frequencyNode.isValid())
        return nullptr;

    juce::Array<double> frequencyTable;
    for (auto child : frequencyNode)
    {
        auto frequency = (double)child[Everytone::ID::Value];
        frequencyTable.add(frequency);
    }

    TuningTable::Definition definition { frequencyTable };

    applyTuningTablePropertiesFromValueTree(tree, 
                                            definition.rootIndex, 
                                            definition.name, 
                                            definition.description, 
                                            definition.periodString, 
                                            definition.virtualPeriod, 
                                            definition.virtualSize);
    return std::make_shared<TuningTable>(definition);
}

static std::shared_ptr<TuningTable> constructTuningFromValueTree(juce::ValueTree tree)
{
    if (!tree.isValid())
        return nullptr;

    auto functionalTuning = constructFunctionalTuningFromValueTree(tree);
    if (functionalTuning != nullptr)
        return functionalTuning;

    return constructTuningTableFromValueTree(tree);
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

static std::shared_ptr<TuningTableMap> constructTuningTableMapFromValueTree(juce::ValueTree tree)
{
    auto patternNode = tree.getChildWithName(Everytone::ID::Pattern);
    const int mapSize = patternNode.getNumChildren();

    if (mapSize == 0)
        return nullptr;

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
        pattern,
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

    return std::make_shared<TuningTableMap>(definition);
}

static juce::ValueTree mappedTuningToValueTree(const MappedTuningTable* mappedTuning, juce::Identifier name)
{
    auto tuningTree = tuningToValueTree(mappedTuning->getTuning());
    auto mappingTree = tuningTableMapToValueTree(mappedTuning->getMapping());

    auto tree = juce::ValueTree(name);
    tree.addChild(tuningTree, 0, nullptr);
    tree.addChild(mappingTree, 1, nullptr);
    return tree;
}