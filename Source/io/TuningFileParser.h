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

#include "../tuning/FunctionalTuning.h"

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
        INV = 0,    // Invalid
        SCL,        // Scala
        TUN,        // Anamark Tun 2.0
    };

public:

    TuningFileParser(juce::File file);
    ~TuningFileParser();

    void readFile(juce::File file);

    std::shared_ptr<TuningTable> getTuning() const;

    TuningType getTuningType() const;

public:
    
    static int determineTuningType(juce::File tuningFileIn);

    static std::shared_ptr<FunctionalTuning> parseScalaFileDefinition(juce::File scalaFile);

    static std::shared_ptr<TuningTable> parseTunFileDefinition(juce::File tunFile);

private:

    void parseTuning(juce::File file);

    static juce::String tunInfoToString(const TUN::CSingleScale& tunScale);

private:

    //juce::String filePath;
    TuningType type = TuningType::INV;
    std::shared_ptr<TuningTable> parsedTuning;
};