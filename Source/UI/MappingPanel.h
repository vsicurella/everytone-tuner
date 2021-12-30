/*
  ==============================================================================

    MappingPanel.h
    Created: 28 Dec 2021 10:51:58pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include "../TuningChanger.h"

//==============================================================================
/*
*/
class MappingPanel  : public juce::Component, public TuningChanger, public OptionsChanger
{
    std::unique_ptr<juce::Label> rootMidiChannelBox;
    std::unique_ptr<juce::Label> rootMidiNoteBox;
    std::unique_ptr<juce::Label> rootFrequencyBox;

    std::unique_ptr<juce::TextButton> linearMappingButton;
    std::unique_ptr<juce::TextButton> periodicMappingButton;

    std::unique_ptr<juce::Label> pitchbendRangeValue;

    juce::String rootChannelBackup;
    juce::String rootNoteBackup;
    juce::String rootFrequencyBackup;

    juce::OwnedArray<juce::Label> labels;

    MappedTuningTable* tuning;

public:
    MappingPanel(Everytone::Options options, MappedTuningTable* tuningIn = nullptr);
    ~MappingPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setTuningDisplayed(const MappedTuningTable* tuning);

    void mappingTypeButtonClicked();

    void tuningReferenceEdited();
    
    void setPitchbendRangeText(int pitchbendRange);
    void setMappedTuning(MappedTuningTable* tuningIn);
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingPanel)
};
