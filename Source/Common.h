/*
  ==============================================================================

    Common.h
    Created: 12 Dec 2021 5:51:24pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

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
		static juce::Identifier VirtualPeriod("VirtualPeriod");
		static juce::Identifier VirtualSize("VirtualSize");
		static juce::Identifier IntervalTable("IntervalTable");
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

		// Options
		static juce::Identifier Options("Options");
		static juce::Identifier MappingMode("MappingMode");
		static juce::Identifier MappingType("MappingType");
		static juce::Identifier ChannelMode("ChannelMode");
		static juce::Identifier MidiMode("MidiMode");
		static juce::Identifier MpeZone("MpeZone");
		static juce::Identifier MpeChannels("MpeChannels");
		static juce::Identifier VoiceRules("VoiceRules");
		static juce::Identifier VoiceLimit("VoiceLimit");
		static juce::Identifier PitchbendRange("PitchbendRange");


		static juce::Identifier Value("Value");
	}


	enum Commands
	{
		Back = 1,
		Save,
		NewTuning,
		OpenTuning,
		NewMapping,
		OpenMapping,
		ShowOptions,
	};

	enum class MappingMode
	{
		Manual = 0,	// Manually set tuning table mapping
		Auto		// Create tuning table mapping based on tuning
	};

	enum class MappingType
	{
		Linear,		// Map root extends linearly in either direction
		Periodic,	// Map root designated start of period and extends linearly up to 128 notes in either direction, then restarts a period multiple away from the root
		Custom
	};

	enum class ChannelMode
	{
		FirstAvailable = 1,	// Finds the first available voice from Channel 1
		RoundRobin,		// Finds the first available voice from last channel assigned
	};

	enum class MpeZone
	{
		Lower = 1,		// Channel 1 is the global channel
		Upper,			// Channel 16 is the global channel
		Omnichannel  	// Legacy mode, use all 16 MIDI channels
	};

	enum class MidiMode
	{
		Mono = 4, // Strictly one note per channel is assigned
		Poly = 3, // Multiple notes on a channel, but only if the pitchbend is the same
	};

	enum class VoiceRule
	{
		Ignore,		// Ignore new notes if voice limit is met
		Overwrite,	// Overwrite oldest note if voice limit is met
	};

	struct Options
	{
		MappingMode mappingMode;
		MappingType mappingType;
		ChannelMode channelMode;
		MpeZone		mpeZone;
		MidiMode	midiMode;
		VoiceRule	voiceRule;
		int			voiceLimit = 16;
		int			pitchbendRange = 96; // Unsure if this should be +/- 2 or MPE default

		juce::ValueTree toValueTree() const
		{
			auto tree = juce::ValueTree(ID::Options);
			tree.setProperty(ID::MappingMode,	(int)mappingMode,	nullptr);
			tree.setProperty(ID::MappingType,	(int)mappingType,	nullptr);
			tree.setProperty(ID::ChannelMode,	(int)channelMode,	nullptr);
			tree.setProperty(ID::MpeZone,		(int)mpeZone,		nullptr);
			tree.setProperty(ID::MidiMode,		(int)midiMode,		nullptr);
			tree.setProperty(ID::VoiceRules,	(int)voiceRule,		nullptr);
			tree.setProperty(ID::VoiceLimit,	(int)voiceLimit,	nullptr);
			tree.setProperty(ID::PitchbendRange,(int)pitchbendRange,nullptr);
			return tree;
		}

		static Options fromValueTree(juce::ValueTree tree)
		{
			return Options
			{
				MappingMode((int)tree[ID::MappingMode]),
				MappingType((int)tree[ID::MappingType]),
				ChannelMode((int)tree[ID::ChannelMode]),
				MpeZone((int)tree[ID::MpeZone]),
				MidiMode((int)tree[ID::MidiMode]),
				VoiceRule((int)tree[ID::VoiceRules]),
				(int)tree[ID::VoiceLimit],
				(int)tree[ID::PitchbendRange]
			};
		}

	};

}


class OptionsChanger;
class OptionsWatcher
{
public:
	virtual void mappingModeChanged(Everytone::MappingMode mode) = 0;
	virtual void mappingTypeChanged(Everytone::MappingType type) = 0;
	virtual void channelModeChanged(Everytone::ChannelMode newChannelMode) = 0;
	virtual void mpeZoneChanged(Everytone::MpeZone zone) = 0;
	virtual void midiModeChanged(Everytone::MidiMode newMidiMode) = 0;
	virtual void voiceLimitChanged(int newVoiceLimit) = 0;
	virtual void pitchbendRangeChanged(int newPitchbendRange) = 0;
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