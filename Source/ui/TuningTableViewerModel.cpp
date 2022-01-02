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

    auto size = tuning->getTableSize();
    rowOutOfBounds.resize(size);
    return size;
}

void TuningTableViewerModel::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (tuning == nullptr)
        return;

    if (rowNumber == tuning->getRootIndex())
        g.fillAll(juce::Colours::steelblue);

    if (rowOutOfBounds[rowNumber])
        g.fillAll(juce::Colours::dimgrey);
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
        auto mts = roundN(4, tuning->mtsAt(rowNumber));
        bool outOfBounds = mts < -0.0f  || mts >= 128;
        rowOutOfBounds.set(rowNumber, outOfBounds);

        switch (column)
        {
        case TuningTableHeader::Columns::Index:
            label->setName("Index" + indexString + "Label");
            label->setText(indexString, juce::NotificationType::dontSendNotification);
            break;
        case TuningTableHeader::Columns::MTS:
        {
            label->setName("Mts" + indexString + "Label");
            label->setText(juce::String(mts), juce::NotificationType::dontSendNotification);
            break;
        }
        case TuningTableHeader::Columns::Frequency:
        {
            label->setName("Frequency" + indexString + "Label");
            auto frequency = roundN(4, tuning->frequencyAt(rowNumber));
            label->setText(juce::String(frequency), juce::NotificationType::dontSendNotification);
            break;
        }
        case TuningTableHeader::Columns::Cents:
        {
            label->setName("Cents" + indexString + "Label");
            auto cents = roundN(3, tuning->centsAt(rowNumber));
            label->setText(juce::String(cents), juce::NotificationType::dontSendNotification);
            break;
        }
        }

        label->setEnabled(!outOfBounds);

    }

    return existingComponentToUpdate;
}
