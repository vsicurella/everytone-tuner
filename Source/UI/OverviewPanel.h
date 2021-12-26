/*
  ==============================================================================

  OverviewPanel.cpp

  Created by Vincenzo Sicurella on 6/30/2020

  ==============================================================================
*/

#pragma once

#include "../tuning/MappedTuning.h"
#include "../TuningChanger.h"

class OverviewPanel  : public juce::Component, public TuningChanger, public OptionsChanger
{
public:
    //==============================================================================
	OverviewPanel(Everytone::Options options = Everytone::Options());
    ~OverviewPanel() override;

public:

	void setTuningDisplayed(const MappedTuning* tuning);

	void mappingTypeButtonClicked();

	void tuningReferenceEdited();

	//==============================================================================

    void paint (juce::Graphics& g) override;
    void resized() override;

private:

	void setTuningNameLabel(juce::String nameIn);
	void setTuningSizeLabel(juce::String numNotesIn);
	void setTuningPeriodLabel(juce::String periodIn);
	void setDescriptionText(juce::String descIn);

	void setPitchbendRangeText(int pitchbendRange);

	void setRootMidiChannelLabel(juce::String channel);
	void setRootMidiNoteLabel(juce::String note);
	void setRootFrequencyLabel(juce::String frequency);

private:

	juce::ValueTree optionsNode;
	juce::Font font;

    //==============================================================================
	
	std::unique_ptr<juce::Label> tuningNameBox;
	std::unique_ptr<juce::Label> tuningSizeBox;
	std::unique_ptr<juce::Label> tuningPeriodBox;

	std::unique_ptr<juce::Label> descriptionTextLabel;
	std::unique_ptr<juce::TextEditor> descriptionEditor;

	std::unique_ptr<juce::Label> rootMidiChannelBox;
	std::unique_ptr<juce::Label> rootMidiNoteBox;
	std::unique_ptr<juce::Label> rootFrequencyBox;

	std::unique_ptr<juce::TextButton> linearMappingButton;
	std::unique_ptr<juce::TextButton> periodicMappingButton;

	std::unique_ptr<juce::Label> pitchbendRangeValue;

	juce::OwnedArray<juce::Label> labels;

#if JUCE_DEBUG
	juce::Array<juce::Rectangle<int>> debugBoxes;
#endif

	juce::String rootChannelBackup;
	juce::String rootNoteBackup;
	juce::String rootFrequencyBackup;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OverviewPanel)
};
