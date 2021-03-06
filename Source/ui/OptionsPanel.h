/*
  ==============================================================================

    OptionsPanel.h
    Created: 19 Dec 2021 4:54:19pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Common.h"
//==============================================================================
/*
*/

class OptionsPanel  : public juce::Component, public OptionsChanger
{
public:
    OptionsPanel(Everytone::Options optionsIn = Everytone::Options());
    ~OptionsPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setPitchbendRangeText(int pitchbendRange);

private:
    
    //Everytone::ControlLookAndFeel controlLookAndFeel;

    std::unique_ptr<juce::ComboBox> channelModeBox;
    std::unique_ptr<juce::ComboBox> channelRulesBox;
    std::unique_ptr<juce::ComboBox> bendModeBox;
    std::unique_ptr<juce::ComboBox> mpeZoneBox;
    std::unique_ptr<LabelMouseHighlight> voiceLimitValueLabel;
    std::unique_ptr<LabelMouseHighlight> pitchbendRangeValue;

    juce::Label* voiceLimitLabel;
    juce::Label* pitchbendRangeLabel;

    juce::OwnedArray<juce::Label> labels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPanel)
};
