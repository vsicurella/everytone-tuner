/*
  ==============================================================================

  OverviewPanel.cpp

  Created by Vincenzo Sicurella on 6/30/2020

  ==============================================================================
*/

#pragma once

#include "../tuning/MappedTuning.h"
#include "../TuningChanger.h"
#include "ListEditor.h"

class OverviewPanel  : public juce::Component, public TuningChanger, public OptionsChanger
{
public:
    //==============================================================================
	OverviewPanel(Everytone::Options options = Everytone::Options());
    ~OverviewPanel() override;

public:

	void setTuningDisplayed(const MappedTuning* tuning);

	void setListEditorModel(ListEditor* listEditor);

	//==============================================================================

    void paint (juce::Graphics& g) override;
    void resized() override;

private:

	juce::ValueTree optionsNode;
	juce::Font font;

    //==============================================================================
	
	std::unique_ptr<juce::Label> tuningNameBox;
	std::unique_ptr<juce::Label> tuningSizeBox;
	std::unique_ptr<juce::Label> tuningPeriodBox;

	std::unique_ptr<juce::Label> descriptionTextLabel;
	std::unique_ptr<juce::TextEditor> descriptionEditor;

	std::unique_ptr<ListEditorHeader> listEditorHeader;
	std::unique_ptr<juce::TableListBox> listEditorComponent;

	juce::OwnedArray<juce::Label> labels;

#if JUCE_DEBUG
	juce::Array<juce::Rectangle<int>> debugBoxes;
#endif

    //====================================================================w==========
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OverviewPanel)
};
