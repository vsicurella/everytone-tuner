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

namespace Multimapper
{
    namespace ID
    {
        static juce::Identifier State("Multimapper");

        // Tuning
        static juce::Identifier TuningSource("TuningSource");
        static juce::Identifier TuningTarget("TuningTarget");
        static juce::Identifier Tuning("Tuning");
        static juce::Identifier Name("Name");
        static juce::Identifier Description("Description");
        static juce::Identifier Transpose("Transpose");
        static juce::Identifier IntervalTable("IntervalTable");
        static juce::Identifier Cents("Cents");
        static juce::Identifier RootMidiNote("rootMidiNote");
        static juce::Identifier RootMidiChannel("rootMidiChannel");
        static juce::Identifier RootFrequency("rootFrequency");
        static juce::Identifier RootTuningIndex("RootTuningIndex");

        // Mapping
        static juce::Identifier MidiMapping("MidiMapping");
        static juce::Identifier MappingPattern("MappingPattern");
        static juce::Identifier Value("Value");
    }
}

static juce::ValueTree tuningToValueTree(const Tuning& tuning, juce::Identifier name = Multimapper::ID::Tuning)
{
    auto tree = juce::ValueTree(name);
    
    tree.setProperty(Multimapper::ID::Name, tuning.getName(), nullptr);
    tree.setProperty(Multimapper::ID::Description, tuning.getDescription(), nullptr);
    //tree.setProperty(Multimapper::ID::Transpose, tuning.tranpose)

    auto intervalTree = juce::ValueTree(Multimapper::ID::IntervalTable);
    auto intervals = tuning.getIntervalCentsTable();
    for (auto cents : intervals)
    {
        auto node = juce::ValueTree(Multimapper::ID::Cents);
        node.setProperty(Multimapper::ID::Value, cents, nullptr);
        intervalTree.addChild(node, -1, nullptr);
    }

    tree.addChild(intervalTree, 0, nullptr);

    tree.setProperty(Multimapper::ID::RootMidiNote, tuning.getRootMidiNote(), nullptr);
    tree.setProperty(Multimapper::ID::RootMidiChannel, tuning.getRootMidiChannel(), nullptr);
    tree.setProperty(Multimapper::ID::RootFrequency, tuning.getRootFrequency(), nullptr);
    tree.setProperty(Multimapper::ID::RootTuningIndex, tuning.getRootIndex(), nullptr);

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

    definition.name = tree.getProperty(Multimapper::ID::Name, "");
    definition.description = tree.getProperty(Multimapper::ID::Description, "");
    definition.transpose = tree.getProperty(Multimapper::ID::Transpose, 0);
    definition.reference.rootMidiNote = tree.getProperty(Multimapper::ID::RootMidiNote, 60);
    definition.reference.rootMidiChannel = tree.getProperty(Multimapper::ID::RootMidiChannel, 1);
    definition.reference.rootFrequency = tree.getProperty(Multimapper::ID::RootFrequency, 256);

    return Tuning(definition);
}