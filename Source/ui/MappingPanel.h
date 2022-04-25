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
    std::unique_ptr<juce::GroupComponent> referenceGroup;

    std::unique_ptr<LabelMouseHighlight> rootMidiChannelBox;
    std::unique_ptr<LabelMouseHighlight> rootMidiNoteBox;
    std::unique_ptr<juce::TextButton> linearMappingButton;
    std::unique_ptr<juce::TextButton> periodicMappingButton;
    std::unique_ptr<juce::GroupComponent> rootGroup;

    std::unique_ptr<juce::Label> mappingRootFrequencyLabel;

    std::unique_ptr<juce::TextButton> lockReferenceButton;

    std::unique_ptr<LabelMouseHighlight> rootFrequencyBox;
    std::unique_ptr<juce::GroupComponent> frequencyGroup;

    std::unique_ptr<juce::Slider> mtsNoteSlider;
    std::unique_ptr<juce::TextButton> mtsSnapButton;
    std::unique_ptr<juce::GroupComponent> mtsSnapGroup;

    int refChannelBackup;
    int refNoteBackup;
    double rootFrequencyBackup;
    int rootChannelBackup;
    int rootNoteBackup;

    juce::OwnedArray<juce::Label> labels;
    juce::Label* rootChannelLabel;
    juce::Label* mtsNoteLabel;

    const MappedTuningTable* tuning;

public:
    MappingPanel(Everytone::Options options, MappedTuningTable* tuningIn = nullptr);
    ~MappingPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setTuningDisplayed(const MappedTuningTable* tuning);

    // Control Callbacks
    void lockReferenceButtonClicked();
    void mappingTypeButtonClicked();
    void tuningReferenceEdited();
    void tuningRootFrequencyEdited();
    void mappingRootEdited();
    void mtsSnapNoteChanged();
    void snapButtonClicked();
        
private:

    void setLockState(bool isLocked, bool sendChangeMessage);

    juce::String getFrequencyGroupTitle() const;

    static juce::Point<float> getComponentMidPointEdge(juce::Component& component, bool leftEdge);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappingPanel)
};
