/*
  ==============================================================================

    TuningFileParser.h
    Created: 23 Jul 2020 8:04:50pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <iostream>;

#include <JuceHeader.h>

#include "../tuning/Tuning.h"

#include "TUN_V2/SCL_Import.h"
#include "TUN_V2/TUN_Scale.h"
#include "TUN_V2/TUN_MultiScaleFile.h"
#include "TUN_V2/TUN_Formula.h"
#include "TUN_V2/TUN_EmbedHTML.h"


class TuningFileParser
{

public:

	// TODO: move somewhere more common?

	enum TuningType
	{
		INV = 0,	// Invalid
		SCL,		// Scala
		TUN,		// Anamark Tun 2.0
	};

public:

	TuningFileParser() {};
	TuningFileParser(juce::String filepath);
	TuningFileParser(juce::File file);
	~TuningFileParser();

	void loadFile(juce::String absoluteFilePath);

	void readFile(juce::File file);

	CentsDefinition getTuningDefinition() const;

	TuningType getTuningType() const;

public:
	
	static int determineTuningType(juce::File tuningFileIn);

	static CentsDefinition parseScalaFileDefinition(juce::File scalaFile);

	static CentsDefinition parseTunFileDefinition(juce::File tunFile);

private:

	void parseTuning(juce::File file);

private:

	//juce::String filePath;
	TuningType type = TuningType::INV;
	std::unique_ptr<CentsDefinition> tuningDefinition;
};