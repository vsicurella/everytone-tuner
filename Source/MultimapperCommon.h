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
		Manual = 0,
		Auto
	};

	enum class MappingType
	{
		Linear,
		Periodic,
		Custom
	};
}
