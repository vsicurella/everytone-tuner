/*
  ==============================================================================

    TuningTableViewer.cpp
    Created: 29 Dec 2021 7:17:54pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TuningTableViewerModel.h"

TuningTableViewerModel::TuningTableViewerModel(const TuningTable* tuningIn)
{
    setTuning(tuningIn);
}

void TuningTableViewerModel::setTuning(const TuningTable* tuningIn)
{
    tuning = tuningIn;
}

int  TuningTableViewerModel::getNumRows()
{
    if (tuning == nullptr)
        return 0;

    return tuning->getTuningTableSize();
}

void TuningTableViewerModel::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (tuning == nullptr)
        return;

    if (rowNumber == tuning->getRootIndex())
        g.fillAll(juce::Colours::steelblue);
}

void TuningTableViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (tuning == nullptr)
        return;
}

juce::Component* TuningTableViewerModel::refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, juce::Component* existingComponentToUpdate)
{
    if (tuning == nullptr)
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

    auto column = TuningTableHeader::Columns(columnId);
    auto indexString = juce::String(rowNumber);

    auto label = dynamic_cast<juce::Label*>(existingComponentToUpdate);
    if (label != nullptr)
    {
        switch (column)
        {
        case TuningTableHeader::Columns::Index:
            label->setName("Index" + indexString + "Label");
            label->setText(indexString, juce::NotificationType::dontSendNotification);
            break;
        case TuningTableHeader::Columns::MTS:
        {
            label->setName("Mts" + indexString + "Label");
            auto mts = tuning->mtsAt(rowNumber);
            label->setText(juce::String(mts), juce::NotificationType::dontSendNotification);
            break;
        }
        case TuningTableHeader::Columns::Frequency:
        {
            label->setName("Frequency" + indexString + "Label");
            auto frequency = tuning->frequencyAt(rowNumber);
            label->setText(juce::String(frequency), juce::NotificationType::dontSendNotification);
            break;
        }
        }
    }

    return existingComponentToUpdate;
}
