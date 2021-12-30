/*
  ==============================================================================

    MappingTableModel.h
    Created: 29 Dec 2021 8:08:34pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../mapping/TuningTableMap.h"
#include "../tuning/TuningBase.h"

class MappingTableHeader : public juce::TableHeaderComponent
{
public:

    enum class Columns
    {
        Channel = 1,
        Note,
        TuningTableIndex,
        MTS,
        Frequency
    };

    MappingTableHeader()
    {
        addColumn("Ch", (int)Columns::Channel, 48);
        addColumn("Note", (int)Columns::Note, 48);
        addColumn("Tuning Index", (int)Columns::TuningTableIndex, 72);
    }
};

class MappingTableModel : public juce::TableListBoxModel
{
    const TuningTableMap* mapping = nullptr;

public:

    MappingTableModel(const TuningTableMap* mappingIn = nullptr);

    void setMapping(const TuningTableMap* tuningIn);

    int getNumRows() override;

    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, juce::Component* existingComponentToUpdate) override;
};
