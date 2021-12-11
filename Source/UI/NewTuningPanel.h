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

    std::unique_ptr<juce::TextButton> saveButton;
    std::unique_ptr<juce::TextButton> backButton;
    std::unique_ptr<juce::TextButton> previewButton;

    std::unique_ptr<Tuning> tuningStaged;

    int lastTabIndex = 1;

public:

    enum NewTuningTabs
    {
        EqualTemperament = 0,
        Rank2Temperament,
    };

    NewTuningPanel(juce::ApplicationCommandManager* cmdManager);
    ~NewTuningPanel() override;

    
    bool previewOn() const { return previewButton->getToggleState(); }
    const Tuning* stagedTuning() const { return tuningStaged.get(); }

    void saveTuning();

    // juce::Component implementation
    void resized() override;

    // TuningWatcher Implementation
    void tuningChanged(TuningChanger* changer, Tuning* tuning) override;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewTuningPanel)
};