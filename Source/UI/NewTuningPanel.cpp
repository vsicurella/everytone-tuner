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

    listTuningInterface = std::make_unique<NewListTuningInterface>("ListTuningInterface");
    addChildComponent(*listTuningInterface);
    listTuningInterface->addTuningWatcher(this);

    saveButton.reset(new juce::TextButton(juce::translate("Save"), juce::translate("Confirm new tuning and go back to main window.")));
    //saveButton->setCommandToTrigger(cmdManager, Everytone::Commands::Save, false);
    saveButton->onClick = [&]() { saveTuning(); };
    addAndMakeVisible(*saveButton);

    backButton.reset(new juce::TextButton(juce::translate("Cancel"), juce::translate("Cancel tuning changes and go back to main window.")));
    backButton->setCommandToTrigger(cmdManager, Everytone::Commands::Back, false);
    addAndMakeVisible(*backButton);

    previewButton.reset(new juce::TextButton(juce::translate("Preview"), juce::translate("Enable tuning changes while you edit")));
    previewButton->setClickingTogglesState(true);
    //addAndMakeVisible(*previewButton);

    toneCircle = std::make_unique<ToneCircle>("NewTuningToneCircle");
    addAndMakeVisible(*toneCircle);

    addTab("Equal", juce::Colours::transparentBlack, equalTemperamentInterface.get(), false, NewTuningTabs::EqualTemperament);
    addTab("List", juce::Colours::transparentBlack, listTuningInterface.get(), false, NewTuningTabs::IntervalList);

    setCurrentTabIndex(NewTuningTabs::EqualTemperament, true);
}

NewTuningPanel::~NewTuningPanel()
{
    previewButton = nullptr;
    backButton = nullptr;
    saveButton = nullptr;
    listTuningInterface = nullptr;
    equalTemperamentInterface = nullptr;
    tuningWatchers.clear();
}

void NewTuningPanel::saveTuning()
{
    tuningWatchers.call(&TuningWatcher::targetDefinitionLoaded, this, definitionStaged);
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
        contentComponent->setBounds(getLocalBounds().withTop(tabs->getBottom()).withBottom(buttonTop).withRight(w / 2));
    }

    toneCircle->setBounds(getLocalBounds().withTop(tabs->getBottom()).withLeft(w / 2));

    double buttonWidth = w * 0.25;

    backButton->setBounds(0, buttonTop, buttonWidth, eighthHeight);
    saveButton->setBounds(backButton->getBounds().translated(buttonWidth, 0));
    //previewButton->setBounds(saveButton->getBounds().translated(buttonWidth, 0));
}

void NewTuningPanel::targetDefinitionLoaded(TuningChanger* changer, CentsDefinition definition)
{
    definitionStaged = definition;
    tuningStaged = std::make_unique<FunctionalTuning>(definition);

    toneCircle->setScale(tuningStaged.get());
    toneCircle->repaint();

    if (getCurrentContentComponent() != listTuningInterface.get())
        listTuningInterface->setIntervalList(definition);
    //if (previewOn())
    //    tuningWatchers.call(&TuningWatcher::targetDefinitionLoaded, this, *tuningStaged);
}
