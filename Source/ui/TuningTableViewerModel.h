/*
  ==============================================================================

    TuningTableViewerModel.h
    Created: 29 Dec 2021 7:17:54pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "../tuning/TuningTable.h"

class TuningTableHeader : public juce::TableHeaderComponent
{
public:

    enum class Columns
    {
        Index = 1,
        MTS,
        Frequency,
        Cents
    };

    TuningTableHeader()
    {
        addColumn("#",   (int)Columns::Index, 48);
        addColumn("MTS", (int)Columns::MTS, 72);
        addColumn("Hz",  (int)Columns::Frequency, 96);
        addColumn("Cents", (int)Columns::Cents, 84);
    }
};

class TuningTableViewerModel : public juce::TableListBoxModel
{
    const TuningTable* tuning = nullptr;

    juce::Array<bool> rowOutOfBounds;

public:

    TuningTableViewerModel(const TuningTable* tuningIn = nullptr);

    void setTuning(const TuningTable* tuningIn);

    int getNumRows() override;

    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, juce::Component* existingComponentToUpdate) override;
};
