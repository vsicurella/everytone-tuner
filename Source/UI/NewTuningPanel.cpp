/*
  ==============================================================================

    NewTuningPanel.cpp
    Created: 9 Dec 2021 8:30:46pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "NewTuningPanel.h"

NewTuningPanel::NewTuningPanel(juce::ApplicationCommandManager* cmdManagerIn)
    : cmdManager(cmdManagerIn), TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtTop)
{
    equalTemperamentInterface.reset(new EqualTemperamentInterface());
    addChildComponent(*equalTemperamentInterface);
    equalTemperamentInterface->addTuningWatcher(this);

    saveButton.reset(new juce::TextButton(juce::translate("Save"), juce::translate("Confirm new tuning and go back to main window.")));
    saveButton->setCommandToTrigger(cmdManager, Everytone::Commands::Save, false);
    addAndMakeVisible(*saveButton);

    backButton.reset(new juce::TextButton(juce::translate("Cancel"), juce::translate("Cancel tuning changes and go back to main window.")));
    backButton->setCommandToTrigger(cmdManager, Everytone::Commands::Back, false);
    addAndMakeVisible(*backButton);

    previewButton.reset(new juce::TextButton(juce::translate("Preview"), juce::translate("Enable tuning changes while you edit")));
    previewButton->setClickingTogglesState(true);
    addAndMakeVisible(*previewButton);

    addTab("Equal", juce::Colours::transparentBlack, equalTemperamentInterface.get(), false, NewTuningTabs::EqualTemperament);

    setCurrentTabIndex(NewTuningTabs::EqualTemperament, true);
}

NewTuningPanel::~NewTuningPanel()
{
    equalTemperamentInterface = nullptr;
    tuningWatchers.clear();
}

void NewTuningPanel::saveTuning()
{
    tuningWatchers.call(&TuningWatcher::tuningTargetChanged, this, tuningStaged.get());
}

void NewTuningPanel::resized()
{    
    juce::TabbedComponent::resized();

    int w = getWidth();
    int h = getHeight();

    double eighthWidth = w * 0.125;
    double eighthHeight = h * 0.125;

    tabs->setBounds(getLocalBounds().withHeight(eighthHeight));

    double buttonTop = h - eighthHeight;

    auto contentComponent = getCurrentContentComponent();
    if (contentComponent != nullptr)
    {
        //contentComponent->setVisible(true);
        contentComponent->setBounds(getLocalBounds().withTop(tabs->getBottom()).withBottom(buttonTop));
    }

    double buttonWidth = w * 0.2;

    backButton->setBounds(0, buttonTop, buttonWidth, eighthHeight);
    saveButton->setBounds(backButton->getBounds().translated(buttonWidth, 0));
    previewButton->setBounds(saveButton->getBounds().translated(buttonWidth, 0));
}

void NewTuningPanel::tuningTargetChanged(TuningChanger* changer, const Tuning* tuning)
{
    tuningStaged.reset(new Tuning(*tuning));

    if (previewOn())
        tuningWatchers.call(&TuningWatcher::tuningTargetChanged, this, tuningStaged.get());
}
