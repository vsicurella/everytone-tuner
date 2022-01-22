/*
  ==============================================================================

    NewListTuningInterface.h
    Created: 19 Jan 2022 10:44:44pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include "../TuningChanger.h"
#include "./IntervalListEditor.h"

class NewListTuningInterface :  public juce::Component,
                                public TuningChanger,
                                public TuningWatcher
{
    IntervalListModel listModel;
    std::unique_ptr<juce::TableListBox> intervalTable;

    CentsDefinition intervalList;

public:

    NewListTuningInterface(juce::String name = juce::String())
        : listModel(true)
    {
        intervalTable = std::make_unique<TableListBox>("IntervalTable", &listModel);
        intervalTable->setHeader(std::make_unique<IntervalListHeader>(true));
        addAndMakeVisible(*intervalTable);

        intervalList.name = "New List Tuning";
        intervalList.description = "";
        intervalList.intervalCents = {};

        listModel.addTuningWatcher(this);
    }

    ~NewListTuningInterface()
    {
        intervalTable = nullptr;
        listModel.removeTuningWatcher(this);
    }

    void paint(juce::Graphics& g) override {}

    void resized() override
    {
        intervalTable->setBounds(getLocalBounds());
    }

    void setIntervalList(const CentsDefinition& definitionIn)
    {
        intervalList.intervalCents = definitionIn.intervalCents;

        listModel.setTuningDefinition(intervalList);
        intervalTable->updateContent();
    }

    // TuningWatcher Implementation
    void targetDefinitionLoaded(TuningChanger* changer, CentsDefinition definition) override
    {
        intervalList = definition;
        setIntervalList(intervalList);
        tuningWatchers.call(&TuningWatcher::targetDefinitionLoaded, this, intervalList);
    }
};