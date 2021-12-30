/*
  ==============================================================================

  OverviewPanel.cpp

  Created by Vincenzo Sicurella on 6/30/2020

  ==============================================================================
*/

#pragma once

#include "../tuning/MappedTuning.h"
#include "../TuningChanger.h"
#include "TuningTableViewer.h"

class OverviewPanel  : public juce::Component, public TuningChanger, public OptionsChanger
{
public:
    //==============================================================================
	OverviewPanel(Everytone::Options options = Everytone::Options());
    ~OverviewPanel() override;

public:

	void setTuningDisplayed(const MappedTuning* tuning);

	void setListEditorModel(IntervalListModel* listEditor);

	//==============================================================================

    void paint (juce::Graphics& g) override;
    void resized() override;

private:

	const MappedTuning* mappedTuning = nullptr;

	std::unique_ptr<juce::Label> tuningNameBox;
	std::unique_ptr<juce::Label> tuningSizeBox;
	std::unique_ptr<juce::Label> tuningPeriodBox;

	std::unique_ptr<juce::Label> descriptionTextLabel;
	std::unique_ptr<juce::TextEditor> descriptionEditor;

	//std::unique_ptr<IntervalListHeader> listEditorHeader;
	//std::unique_ptr<juce::TableListBox> listEditorComponent;
	std::unique_ptr<TuningTableViewer> tuningTableViewer;

	juce::OwnedArray<juce::Label> labels;

#if JUCE_DEBUG
	juce::Array<juce::Rectangle<int>> debugBoxes;
#endif

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OverviewPanel)
};
