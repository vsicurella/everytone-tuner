/*
  ==============================================================================

    MappingTableModel.cpp
    Created: 29 Dec 2021 8:08:34pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "MappingTableModel.h"

MappingTableModel::MappingTableModel(const TuningTableMap* mappingIn)
{
    setMapping(mappingIn);
}

void MappingTableModel::setMapping(const TuningTableMap* mappingIn)
{
    mapping = mappingIn;
}

int MappingTableModel::getNumRows()
{
    if (mapping == nullptr)
        return 0;

    return 2048;
}

void MappingTableModel::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (mapping == nullptr)
        return;

    if (rowNumber == mapping->getRootMidiIndex())
        g.fillAll(juce::Colours::steelblue);
}

void MappingTableModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (mapping == nullptr)
        return;
}

juce::Component* MappingTableModel::refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, juce::Component* existingComponentToUpdate)
{
    if (mapping == nullptr)
    {
        if (existingComponentToUpdate != nullptr)
            delete existingComponentToUpdate;

        return nullptr;
    }

    if (existingComponentToUpdate == nullptr)
    {
        auto label = new juce::Label();
        label->setJustificationType(juce::Justification::centred);
        existingComponentToUpdate = label;
    }

    auto column = MappingTableHeader::Columns(columnId);
    auto indexString = juce::String(rowNumber);

    auto label = dynamic_cast<juce::Label*>(existingComponentToUpdate);
    if (label != nullptr)
    {
        switch (column)
        {
        case MappingTableHeader::Columns::Channel:
        {
            label->setName("Channel" + indexString + "Label");
            int channel = (rowNumber / 128) + 1;
            label->setText(juce::String(channel), juce::NotificationType::dontSendNotification);
            break;
        }
        case MappingTableHeader::Columns::Note:
        {
            label->setName("Note" + indexString + "Label");
            auto note = rowNumber % 128;
            label->setText(juce::String(note), juce::NotificationType::dontSendNotification);
            break;
        }
        case MappingTableHeader::Columns::TuningTableIndex:
        {
            label->setName("TuningIndex" + indexString + "Label");
            auto index = mapping->tableAt(rowNumber);
            label->setText(juce::String(index), juce::NotificationType::dontSendNotification);
            break;
        }
        }
    }

    return existingComponentToUpdate;
}
