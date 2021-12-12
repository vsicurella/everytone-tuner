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

#include "../Tuning.h"

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
	TuningFileParser(const juce::String& absoluteFilePath);
	~TuningFileParser();

	void loadFile(const juce::String& absoluteFilePath);

	void readFile(const juce::File& file);

	Tuning::Definition getTuningDefinition() const;

	const Tuning* getTuning() const;

	TuningType getTuningType() const;

public:
	
	static int determineTuningType(const juce::File& tuningFileIn);

	static Tuning::CentsDefinition parseScalaFileDefinition(const juce::File& scalaFile);

	static Tuning::CentsDefinition parseTunFileDefinition(const juce::File& tunFile);

private:

	void parseTuning(const juce::File& file);

private:

	juce::String filePath;
	TuningType type = TuningType::INV;
	std::unique_ptr<Tuning> tuningLoaded;
};