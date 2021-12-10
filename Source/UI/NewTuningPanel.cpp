/*
  ==============================================================================

    NewTuningPanel.cpp
    Created: 9 Dec 2021 8:30:46pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "NewTuningPanel.h"

NewTuningPanel::NewTuningPanel(juce::ApplicationCommandManager* cmdManagerIn)
    : TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtTop), cmdManager(cmdManagerIn)
{
    equalTemperamentInterface.reset(new EqualTemperamentInterface());

    equalTemperamentInterface->addTuningWatcher(this);
    addTab("Back", juce::Colours::transparentBlack, nullptr, false, NewTuningTabs::Back);
    addTab("Equal", juce::Colours::darkslateblue, equalTemperamentInterface.get(), false, NewTuningTabs::EqualTemperament);

    setCurrentTabIndex(NewTuningTabs::EqualTemperament, false);
}

NewTuningPanel::~NewTuningPanel()
{
    equalTemperamentInterface = nullptr;

    tuningWatchers.clear();
}

void NewTuningPanel::currentTabChanged(int newCurrentTabIndex, const juce::String& newCurrentTabName)
{
    switch (newCurrentTabIndex)
    {
    case NewTuningTabs::Back:
        cmdManager->invokeDirectly(Multimapper::Commands::Back, true);
        break;

    case NewTuningTabs::EqualTemperament:
        
        break;

    default:
        jassertfalse;
        // back
    }
}

//void NewTuningPanel::resized()
//{
//
//}

void NewTuningPanel::tuningChanged(TuningChanger* changer, Tuning* tuning)
{
    tuningWatchers.call(&TuningWatcher::tuningChanged, this, tuning);
}