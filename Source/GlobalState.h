/*
  ==============================================================================

    GlobalState.h
    Created: 11 Dec 2021 1:52:10pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Tuning.h"

class GlobalState
{

public:

    GlobalState() {}

    virtual ~GlobalState() {}

    virtual const Tuning* activeSourceTuning() const = 0;
    virtual const Tuning* activeTargetTuning() const = 0;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GlobalState)

};

#define GetMultimapperState() std::dynamic_pointer_cast<GlobalState*>(std::dynamic_pointer_cast<juce::StandaloneFilterWindow*>(juce::JUCEApplication::getInstance())->mainWindow)