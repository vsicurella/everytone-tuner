/*
  ==============================================================================

  OverviewPanel.cpp

  Created by Vincenzo Sicurella on 6/30/2020

  ==============================================================================
*/

#pragma once

#include "CommonUI.h"
#include "../Tuning.h"
#include "../TuningChanger.h"

class OverviewPanel  : public juce::Component, public TuningWatcher
{
public:
    //==============================================================================
	OverviewPanel();
    ~OverviewPanel() override;

public:

	void setTuningDisplayed(const Tuning& tuning);

	void setTuningNameLabel(juce::String nameIn);
	void setTuningSizeLabel(juce::String numNotesIn);
	void setTuningPeriodLabel(juce::String periodIn);
	void setDescriptionText(juce::String descIn);

	//==============================================================================

    void paint (juce::Graphics& g) override;
    void resized() override;

	//==============================================================================
	// TuningWatcher implementation

	void tuningChanged(TuningChanger* changer, Tuning* tuning) override;

private:

	juce::ValueTree optionsNode;
	juce::Font font;

    //==============================================================================
	
	std::unique_ptr<juce::Label> tuningNameBox;
	std::unique_ptr<juce::Label> tuningSizeBox;
	std::unique_ptr<juce::Label> tuningPeriodBox;

	std::unique_ptr<juce::Label> descriptionTextLabel;
	std::unique_ptr<juce::TextEditor> descriptionEditor;

	juce::OwnedArray<juce::Label> labels;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OverviewPanel)
};
