/*
  ==============================================================================

    OptionsPanel.h
    Created: 19 Dec 2021 4:54:19pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../MultimapperCommon.h"

//==============================================================================
/*
*/

class OptionsPanel  : public juce::Component, public OptionsChanger
{
public:
    OptionsPanel(Multimapper::Options optionsIn = Multimapper::Options());
    ~OptionsPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    juce::OwnedArray<juce::Label> labels;

    std::unique_ptr<juce::ComboBox> channelModeBox;
    std::unique_ptr<juce::ComboBox> channelRulesBox;
    
    std::unique_ptr<juce::Label> voiceLimitValueLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPanel)
};
