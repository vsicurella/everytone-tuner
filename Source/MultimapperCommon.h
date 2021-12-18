/*
  ==============================================================================

    MultimapperCommon.h
    Created: 12 Dec 2021 5:51:24pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

namespace Multimapper
{
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
		Mono = 1,		// Voice limit of 1
		FirstAvailable,	// Finds the first available voice from Channel 1
		RoundRobin,		// Finds the first available voice from last channel assigned
	};

	enum class MpeZone
	{
		Omnichannel,	// Legacy mode, use all 16 MIDI channels
		Lower,			// Channel 1 is the global channel
		Upper			// Channel 16 is the global channel
	};

	enum class MidiMode
	{
		Mono = 4, // Strictly one note per channel is assigned
		Poly = 3, // Multiple notes on a channel, but only if the pitchbend is the same
	};

	enum class VoiceLimit
	{
		Ignore,		// Ignore new notes if voice limit is met
		Overwrite,	// Overwrite oldest note if voice limit is met
	};
}
