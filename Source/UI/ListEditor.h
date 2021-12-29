/*
  ==============================================================================

    ListEditor.h
    Created: 28 Dec 2021 9:03:41pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "../TuningChanger.h"
#include "../tuning/MappedTuning.h"

class ListEditorHeader : public juce::TableHeaderComponent
{
public:
    enum class Columns
    {
        Index = 1,
        Interval,
        Type,
        InsertAbove,
        Delete
    };

    ListEditorHeader(bool inEditMode);
};

class ListEditor : public juce::TableListBoxModel, public TuningChanger
{
    bool inEditMode = false;

    const TuningBase* tuning;

    CentsDefinition definition;



public:

    ListEditor(bool inEditMode = false, const TuningBase* tuning = nullptr);

    void setTuning(const TuningBase* tuning);

    void sendCentsDefinitionUpdateChange();

    // juce::TablelistModel implemetnation
   
    int getNumRows() override;

    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, juce::Component* existingComponentToUpdate) override;
};