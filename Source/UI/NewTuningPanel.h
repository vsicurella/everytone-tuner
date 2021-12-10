/*
  ==============================================================================

    NewTuningPanel.h
    Created: 9 Dec 2021 8:30:46pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include "../TuningChanger.h"
#include "EqualTemperamentInterface.h"
#include "CommonUI.h"

class NewTuningPanel : public juce::TabbedComponent,
                       public TuningWatcher, 
                       public TuningChanger
{
    juce::ApplicationCommandManager* cmdManager;

    std::unique_ptr<EqualTemperamentInterface> equalTemperamentInterface;

public:

    enum NewTuningTabs
    {
        Back = 0,
        EqualTemperament,
        Rank2Temperament,
    };

    NewTuningPanel(juce::ApplicationCommandManager* cmdManager);
    ~NewTuningPanel() override;

    void currentTabChanged(int newCurrentTabIndex, const juce::String& newCurrentTabName) override;

    // TuningWatcher Implementation
    void tuningChanged(TuningChanger* changer, Tuning* tuning) override;

    //void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewTuningPanel)
};