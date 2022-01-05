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
    std::unique_ptr<LabelMouseHighlight> referenceMidiChannelBox;
    std::unique_ptr<LabelMouseHighlight> referenceMidiNoteBox;
    std::unique_ptr<LabelMouseHighlight> rootFrequencyBox;
    std::unique_ptr<GroupComponent> referenceGroup;

    std::unique_ptr<TextButton> lockReferenceButton;

    std::unique_ptr<LabelMouseHighlight> rootMidiChannelBox;
    std::unique_ptr<LabelMouseHighlight> rootMidiNoteBox;
    std::unique_ptr<GroupComponent> rootGroup;

    std::unique_ptr<juce::TextButton> linearMappingButton;
    std::unique_ptr<juce::TextButton> periodicMappingButton;

    int refChannelBackup;
    int refNoteBackup;
    double rootFrequencyBackup;
    int rootChannelBackup;
    int rootNoteBackup;

    juce::OwnedArray<juce::Label> labels;

    MappedTuningTable* tuning;

public:
    MappingPanel(Everytone::Options options, MappedTuningTable* tuningIn = nullptr);
    ~MappingPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setTuningDisplayed(const MappedTuningTable* tuning);

    void lockReferenceButtonClicked();

    void mappingTypeButtonClicked();

    void tuningReferenceEdited();

    void tuningRootFrequencyEdited();

    void mappingRootEdited();
        
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingPanel)
};
