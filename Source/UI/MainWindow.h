/*
  ==============================================================================

  MainWindow.cpp

  Created by Vincenzo Sicurella on 6/30/2020

  ==============================================================================
*/

#pragma once

#include "CommonUI.h"


class MainWindow  : public juce::Component
{
public:
    //==============================================================================
	MainWindow();
    ~MainWindow() override;

public:

	void loadOptionsNode(juce::ValueTree optionsNodeIn);

	void updateTuningOutProperties();

	void setTuningNameLabel(juce::String nameIn);
	void setTuningSizeLabel(juce::String numNotesIn);
	void setTuningPeriodLabel(juce::String periodIn);
	void setDescriptionText(juce::String descIn);

	//==============================================================================

    void paint (juce::Graphics& g) override;
    void resized() override;

	//==============================================================================

private:

	juce::ValueTree optionsNode;
	juce::Font font;

    //==============================================================================
	
	std::unique_ptr<juce::Label> tuningNameLabel;
	std::unique_ptr<juce::Label> tuningNameBox;
	std::unique_ptr<juce::Label> tuningSizeLabel;
	std::unique_ptr<juce::Label> tuningSizeBox;
	std::unique_ptr<juce::Label> tuningPeriodLabel;
	std::unique_ptr<juce::Label> tuningPeriodBox;

	std::unique_ptr<juce::TextEditor> descriptionTextBox;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};
