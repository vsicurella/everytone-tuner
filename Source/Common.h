/*
  ==============================================================================

    Common.h
    Created: 12 Dec 2021 5:51:24pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template <typename ARR>
static juce::ValueTree arrayToValueTree(ARR array, juce::Identifier id, juce::Identifier nodeId, juce::Identifier valueId)
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

namespace Everytone
{
    namespace ID
    {
        static juce::Identifier State("Everytone");

        // Tuning
        static juce::Identifier TuningSource("TuningSource");
        static juce::Identifier TuningTarget("TuningTarget");
        static juce::Identifier Tuning("Tuning");
        static juce::Identifier Name("Name");
        static juce::Identifier Description("Description");
        static juce::Identifier PeriodString("PeriodString");
        static juce::Identifier VirtualPeriod("VirtualPeriod");
        static juce::Identifier VirtualSize("VirtualSize");
        static juce::Identifier FrequencyTable("FrequencyTable");
        static juce::Identifier Frequency("Frequency");
        static juce::Identifier CentsTable("CentsTable");
        static juce::Identifier Cents("Cents");
        static juce::Identifier RootMidiNote("rootMidiNote");
        static juce::Identifier RootMidiChannel("rootMidiChannel");
        static juce::Identifier RootFrequency("rootFrequency");

        // Mapping
        static juce::Identifier TuningTableMidiMap("TuningTableMidiMap");
        static juce::Identifier RootTuningIndex("RootTuningIndex");
        static juce::Identifier Period("MapPeriod");
        static juce::Identifier Pattern("Pattern");
        static juce::Identifier PatternRoot("PatternRoot");
        static juce::Identifier MapRoot("MapRoot");
        static juce::Identifier Transpose("Transpose");

        // MappedTuning
        static juce::Identifier ReferenceMidiChannel("ReferenceMidiChannel");
        static juce::Identifier ReferenceMidiNote("ReferenceMidiNote");

        // Options
        static juce::Identifier Options("Options");
        static juce::Identifier MappingMode("MappingMode");
        static juce::Identifier MappingType("MappingType");
        static juce::Identifier ChannelMode("ChannelMode");
        static juce::Identifier MidiMode("MidiMode");
        static juce::Identifier MpeZone("MpeZone");
        static juce::Identifier MpeChannels("MpeChannels");
        static juce::Identifier NotePriority("NotePriority");
        static juce::Identifier BendMode("BendMode");
        static juce::Identifier PitchbendRange("PitchbendRange");
        static juce::Identifier DisabledChannels("DisabledChannels");
        static juce::Identifier MidiChannel("MidiChannel");

        static juce::Identifier Value("Value");
    }


    enum Commands
    {
        Back = 1,
        ShowMenu,
        NewTuning,
        OpenTuning,
        EditReference,
        ShowOptions,
    };

    enum class MappingMode
    {
        Manual = 0, // Manually set tuning table mapping
        Auto        // Create tuning table mapping based on tuning
    };

    enum class MappingType
    {
        Linear,      // Map root extends linearly in either direction
        Periodic,    // Map root designated start of period and extends linearly up to 128 notes in either direction, then restarts a period multiple away from the root
        Custom
    };

    static juce::String getTooltip(MappingType mappingType)
    {
        switch (mappingType)
        {
        case MappingType::Linear:
            return juce::String("MIDI notes extend linearly from the root channel & note. "
                                "Tuning table indices will wraparound when out of bounds.");

        case MappingType::Periodic:
            return juce::String("MIDI notes are offset by one period of the tuning per channel away from root channel. "
                                "Notes extend linearly between the root notes of each channel. "
                                "Tuning table indicies will wraparound when out of bounds.");

        case MappingType::Custom:
            return juce::String("Custom mapping scheme.");

        default:
            jassertfalse;
            return juce::String();
        }
    }

    enum class ChannelMode
    {
        FirstAvailable = 1, // Finds the first available voice from Channel 1
        RoundRobin,         // Finds the first available voice from last channel assigned
        Monophonic          // Only one voice is tuned on the same channel
    };

    static juce::String getTooltip(ChannelMode channelMode)
    {
        switch (channelMode)
        {
        case ChannelMode::FirstAvailable:
            return juce::String("Finds the first available MIDI Channel starting from Channel 1.");

        case ChannelMode::RoundRobin:
            return juce::String("Finds the next MIDI channel available, starting from the last assigned channel, and wrapping around after Channel 16.");

        case ChannelMode::Monophonic:
            return juce::String("Only use one MIDI channel, for use with monophonic synthesizers.");

        default:
            jassertfalse;
            return juce::String();
        }
    }

    enum class MpeZone
    {
        Lower = 1,      // Channel 1 is the global channel
        Upper,          // Channel 16 is the global channel
        Omnichannel,    // Legacy mode, use all 16 MIDI channels
    };

    static juce::String getTooltip(MpeZone zone)
    {
        switch (zone)
        {
        case MpeZone::Lower:
            return juce::String("Reserve MIDI Channel 1 for global pitchbend and controller messages, and use channels 2-16 for tuned voices.");

        case MpeZone::Upper:
            return juce::String("Reserve MIDI Channel 16 for global pitchbend and controller messages, and use channels 1-15 for tuned voices.");

        case MpeZone::Omnichannel:
            return juce::String("Use all 16 MIDI channels without reserving a global channel");

        default:
            jassertfalse;
            return juce::String();
        }
    }

    enum class MidiMode
    {
        Mono = 4, // Strictly one note per channel is assigned
        Poly = 3, // Multiple notes on a channel, but only if the pitchbend is the same
    };

    enum class NotePriority
    {
        Lowest,     // Create note-ons for the lowest held notes
        Highest,    // Create note-ons for the highest held notes
        Last,       // Create note-ons for each played note, and when voice becomes available, retrigger previous note
    };

    static juce::String getTooltip(NotePriority notePriority)
    {
        switch (notePriority)
        {
        case NotePriority::Lowest:
            return juce::String("After all voices are used, allow a new lowest note to be triggered by stealing the highest held note.");

        case NotePriority::Highest:
            return juce::String("After all voices are used, allow a new highest note to be triggered by stealing the lowest held note.");

        case NotePriority::Last:
            return juce::String("After all voices are used, allow a new note to be triggered by stealing the oldest held note.");

        default:
            jassertfalse;
            return juce::String();
        }
    }

    // TODO Mode for retriggering or bending (given pitchbend range limits)

    enum class BendMode
    {
        Static = 1,     // Send one pitchbend per note Note On
        Persistent,     // Send pitchbend messages while notes are on
        Dynamic         // Send pitchbend messages to active notes when tuning changes
    };

    static juce::String getTooltip(BendMode bendMode)
    {
        switch (bendMode)
        {
        case BendMode::Static:
            return juce::String("Only send one pitchbend message per note.");

        case BendMode::Persistent:
            return juce::String("Send a stream of pitchbend messages while notes are on.");

        default:
            jassertfalse;
            return juce::String();
        }
    }

    static juce::String getPitchbendRangeTooltip(int pitchbendRange)
    {
        return juce::String("Total pitchbend range of ") + juce::String(pitchbendRange) + juce::String(" semitones.");
    }

    struct Options
    {
        MappingMode     mappingMode     = MappingMode::Auto;
        MappingType     mappingType     = MappingType::Linear;
        ChannelMode     channelMode     = ChannelMode::FirstAvailable;
        MpeZone         mpeZone         = MpeZone::Lower;
        MidiMode        midiMode        = MidiMode::Mono;
        NotePriority    notePriority    = NotePriority::Last;
        BendMode        bendMode        = BendMode::Static;
        int             pitchbendRange  = 4; // Unsure if this should be +/- 2 or MPE default
        juce::Array<bool> disabledChannels = juce::Array<bool>();

        static juce::ValueTree disabledChannelsToValueTree(juce::Array<bool> disabledChannels)
        {
            auto tree = arrayToValueTree(disabledChannels, Everytone::ID::DisabledChannels, Everytone::ID::MidiChannel, Everytone::ID::Value);
            return tree;
        }

        static juce::Array<bool> disabledChannelsFromValueTree(juce::ValueTree disabledChannelsTree)
        {
            if (!disabledChannelsTree.hasType(Everytone::ID::DisabledChannels))
                return juce::Array<bool>();

            juce::Array<bool> disabledChannels;
            for (auto child : disabledChannelsTree)
                disabledChannels.add((bool)child[ID::Value]);

            return disabledChannels;
        }

        juce::ValueTree toValueTree() const
        {
            auto tree = juce::ValueTree(ID::Options);
            tree.setProperty(ID::MappingMode,       (int)mappingMode,       nullptr);
            tree.setProperty(ID::MappingType,       (int)mappingType,       nullptr);
            tree.setProperty(ID::ChannelMode,       (int)channelMode,       nullptr);
            tree.setProperty(ID::MpeZone,           (int)mpeZone,           nullptr);
            tree.setProperty(ID::MidiMode,          (int)midiMode,          nullptr);
            tree.setProperty(ID::BendMode,          (int)bendMode,          nullptr);
            tree.setProperty(ID::NotePriority,      (int)notePriority,      nullptr);
            tree.setProperty(ID::PitchbendRange,    (int)pitchbendRange,    nullptr);
            
            auto disabledChannelsTree = disabledChannelsToValueTree(disabledChannels);
            tree.addChild(disabledChannelsTree, 0, nullptr);

            return tree;
        }

        static Options fromValueTree(juce::ValueTree tree)
        {
            Options options;
            if (tree.hasType(ID::Options))
            {
                if (tree.hasProperty(ID::MappingMode))      options.mappingMode      = MappingMode   ((int)tree[ID::MappingMode]);
                if (tree.hasProperty(ID::MappingType))      options.mappingType      = MappingType   ((int)tree[ID::MappingType]);
                if (tree.hasProperty(ID::ChannelMode))      options.channelMode      = ChannelMode   ((int)tree[ID::ChannelMode]);
                if (tree.hasProperty(ID::MpeZone))          options.mpeZone          = MpeZone       ((int)tree[ID::MpeZone]);
                if (tree.hasProperty(ID::MidiMode))         options.midiMode         = MidiMode      ((int)tree[ID::MidiMode]);
                if (tree.hasProperty(ID::NotePriority))     options.notePriority     = NotePriority  ((int)tree[ID::NotePriority]);
                if (tree.hasProperty(ID::BendMode))         options.bendMode         = BendMode      ((int)tree[ID::BendMode]);
                if (tree.hasProperty(ID::PitchbendRange))   options.pitchbendRange   = (int)tree[ID::PitchbendRange];
                
                auto disabledChannelsTree = tree.getChildWithName(ID::DisabledChannels);
                if (disabledChannelsTree.isValid())
                    options.disabledChannels = disabledChannelsFromValueTree(disabledChannelsTree);
            }

            return options;
        }

    };

}

class OptionsChanger;
class OptionsWatcher
{
public:
    virtual void mappingModeChanged(Everytone::MappingMode mode) {};
    virtual void mappingTypeChanged(Everytone::MappingType type) {};
    virtual void channelModeChanged(Everytone::ChannelMode newChannelMode) {};
    virtual void mpeZoneChanged(Everytone::MpeZone zone) {};
    virtual void midiModeChanged(Everytone::MidiMode newMidiMode) {};
    virtual void bendModeChanged(Everytone::BendMode newBendMode) {};
    virtual void pitchbendRangeChanged(int newPitchbendRange) {};
    virtual void disabledChannelsChanged(juce::Array<bool> disabledChannels) {};
};

class OptionsChanger
{
protected:
    juce::ListenerList<OptionsWatcher> optionsWatchers;

public:
    virtual ~OptionsChanger() {}

    void addOptionsWatcher(OptionsWatcher* watcher) { optionsWatchers.add(watcher); }

    void removeOptionsWatcher(OptionsWatcher* watcher) { optionsWatchers.remove(watcher); }
};

class LabelMouseHighlight : public juce::Label
{
    bool mouseIn = false;

public:

    // Tried to do this with LookAndFeel::drawLabel() but couldn't get it working

    LabelMouseHighlight(const juce::String& componentName = juce::String(), const juce::String& labelText = juce::String())
        : juce::Label(componentName, labelText) {}

    virtual ~LabelMouseHighlight() {}

    void paint(juce::Graphics& g) override
    {
        juce::Label::paint(g);

        if (isMouseOver())
        {
            g.setColour(juce::Colours::white.withAlpha(0.15f));
            g.fillRoundedRectangle(getLocalBounds().toFloat(), getHeight() * 0.125f);
        }
        
    }

    void mouseEnter(const juce::MouseEvent& e) override
    {
        if (mouseIn)
            return;

        repaint();
        mouseIn = true;
    }

    void mouseExit(const juce::MouseEvent& e) override
    {
        if (mouseIn)
        {
            repaint();
            mouseIn = false;
        }
    }
};