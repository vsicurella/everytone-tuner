/*
  ==============================================================================

    IntervalListEditor.h
    Created: 28 Dec 2021 9:03:41pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "../TuningChanger.h"
#include "../tuning/MappedTuning.h"

class IntervalListHeader : public juce::TableHeaderComponent
{
public:
    enum class Columns
    {
        Index = 1,
        Interval,
        Type,
        InsertBelow,
        SwapAbove,
        SwapBelow,
        Delete
    };

    IntervalListHeader(bool inEditMode);
};

class IntervalListModel : public juce::TableListBoxModel, public TuningChanger
{
    bool inEditMode = false;

    const FunctionalTuning* tuning;

    CentsDefinition definition;

public:

    IntervalListModel(bool inEditMode = false, const FunctionalTuning* tuning = nullptr);

    void setTuning(const FunctionalTuning* tuning);
    void setTuningDefinition(CentsDefinition definitionIn);

    void sendCentsDefinitionUpdateChange();

    void insertInterval(int indexToInsert, double centsValue);
    void modifyInterval(int indexToModify, double centsValue);
    void removeInterval(int indexToRemove);

    // juce::TablelistModel implemetnation
   
    int getNumRows() override;

    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, juce::Component* existingComponentToUpdate) override;
};