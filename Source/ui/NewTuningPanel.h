/*
  ==============================================================================

    NewTuningPanel.h
    Created: 9 Dec 2021 8:30:46pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include "../TuningChanger.h"
#include "../Common.h"
#include "EqualTemperamentInterface.h"
#include "NewListTuningInterface.h"
#include "ToneCircle.h"


class NewTuningPanel : public juce::TabbedComponent,
                       public TuningWatcher, 
                       public TuningChanger
{
    juce::ApplicationCommandManager* cmdManager;

    std::unique_ptr<EqualTemperamentInterface> equalTemperamentInterface;
    std::unique_ptr<NewListTuningInterface> listTuningInterface;

    std::unique_ptr<juce::TextButton> saveButton;
    std::unique_ptr<juce::TextButton> backButton;
    std::unique_ptr<juce::TextButton> previewButton;

    std::unique_ptr<FunctionalTuning> tuningStaged;
    CentsDefinition definitionStaged;

    std::unique_ptr<ToneCircle> toneCircle;

    int lastTabIndex = 1;

public:

    enum NewTuningTabs
    {
        EqualTemperament = 0,
        Rank2Temperament,
        IntervalList,
    };

    NewTuningPanel(juce::ApplicationCommandManager* cmdManager);
    ~NewTuningPanel() override;

    
    bool previewOn() const { return previewButton->getToggleState(); }
    CentsDefinition stagedTuning() const { return tuningStaged->getDefinition(); }

    void saveTuning();

    // juce::Component implementation
    void resized() override;

    // TuningWatcher Implementation
    void targetDefinitionLoaded(TuningChanger* changer, CentsDefinition definition) override;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewTuningPanel)
};