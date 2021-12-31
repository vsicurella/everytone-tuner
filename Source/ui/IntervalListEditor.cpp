/*
  ==============================================================================

    IntervalListModel.cpp
    Created: 28 Dec 2021 9:03:41pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "IntervalListEditor.h"

IntervalListHeader::IntervalListHeader(bool inEditMode)
{
    addColumn("#",          (int)Columns::Index, 48);
    addColumn("Interval",   (int)Columns::Interval, 96);
    addColumn("Type",       (int)Columns::Type, 72);

    if (inEditMode)
    {
        addColumn("", (int)Columns::InsertAbove, 24);
        addColumn("", (int)Columns::Delete, 24);
    }
}

IntervalListModel::IntervalListModel(bool editMode, const TuningBase* tuningIn)
    : inEditMode(editMode)
{
    setTuning(tuningIn);
}

void IntervalListModel::setTuning(const TuningBase* tuningIn)
{
    //tuning = tuningIn;

    //auto functional

    //if (tuning == nullptr)
    //{
    //    definition = CentsDefinition();
    //    definition.intervalCents = {};
    //    return;
    //}

    //definition = tuning->getDefinition();
}

void IntervalListModel::sendCentsDefinitionUpdateChange()
{
    tuningWatchers.call(&TuningWatcher::targetDefinitionLoaded, this, definition);
}

void IntervalListModel::insertInterval(int indexToInsert, double centsValue)
{
    definition.intervalCents.insert(indexToInsert, centsValue);
    sendCentsDefinitionUpdateChange();
}

void IntervalListModel::modifyInterval(int indexToModify, double centsValue)
{
    definition.intervalCents.set(indexToModify, centsValue);
    sendCentsDefinitionUpdateChange();
}

void IntervalListModel::removeInterval(int indexToRemove)
{
    definition.intervalCents.removeRange(indexToRemove, 1);
    sendCentsDefinitionUpdateChange();
}


// juce::TablelistModel implemetnation

int IntervalListModel::getNumRows()
{
    if (tuning == nullptr)
        return 0;

    return definition.intervalCents.size();
}

void IntervalListModel::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{

}

void IntervalListModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{

}

juce::Component* IntervalListModel::refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, juce::Component* existingComponentToUpdate)
{
    auto column = IntervalListHeader::Columns(columnId);
    auto indexString = juce::String(rowNumber + 1);

    if (existingComponentToUpdate == nullptr)
    {
        switch (column)
        {
        case IntervalListHeader::Columns::Index:
        {
            auto indexLabel = new juce::Label();
            indexLabel->setJustificationType(juce::Justification::centred);
            existingComponentToUpdate = indexLabel;
            break;
        }
        case IntervalListHeader::Columns::Interval:
        {
            auto intervalLabel = new juce::Label();
            intervalLabel->setJustificationType(juce::Justification::centred);
            intervalLabel->onTextChange = [&, intervalLabel, rowNumber]()
            {
                double newInterval = intervalLabel->getText().getDoubleValue();
                modifyInterval(rowNumber, newInterval);
            };
            existingComponentToUpdate = intervalLabel;
            break;
        }
        case IntervalListHeader::Columns::Type:
        {
            auto typeLabel = new juce::Label();
            typeLabel->setText("cents", juce::NotificationType::dontSendNotification);
            existingComponentToUpdate = typeLabel;
            return typeLabel;
        }
        case IntervalListHeader::Columns::InsertAbove:
        {
            auto insertButton = new juce::TextButton("Insert" + indexString + "Button", "Insert a new interval above this one");
            insertButton->setButtonText("^");
            insertButton->onClick = [&, rowNumber]()
            {
                insertInterval(rowNumber, definition.intervalCents[rowNumber]);
                sendCentsDefinitionUpdateChange();
            };
            return insertButton;
        }
        case IntervalListHeader::Columns::Delete:
        {
            auto deleteButton = new juce::TextButton("Delete" + indexString + "Button", "Remove this interval");
            deleteButton->setButtonText("x");
            deleteButton->onClick = [&, rowNumber]()
            {
                removeInterval(rowNumber);
            };
            return deleteButton;
        }
        default:
            jassertfalse;
            return nullptr;
        }
    }
    
    switch (column)
    {
    case IntervalListHeader::Columns::Index:
    {
        auto label = dynamic_cast<juce::Label*>(existingComponentToUpdate);
        label->setName("Index" + indexString + "Label");
        label->setText(indexString, juce::NotificationType::dontSendNotification);
        return label;
    }
    case IntervalListHeader::Columns::Interval:
    {
        auto label = dynamic_cast<juce::Label*>(existingComponentToUpdate);
        auto intervalString = juce::String(definition.intervalCents[rowNumber]);
        label->setName("Interval" + indexString + "Label");
        label->setText(intervalString, juce::NotificationType::dontSendNotification);
        label->setEditable(inEditMode, false, true);
        return label;
    }
    case IntervalListHeader::Columns::Type:
    {
        auto label = dynamic_cast<juce::Label*>(existingComponentToUpdate);
        label->setName("Type" + indexString + "Label");
        label->setText("cents", juce::NotificationType::dontSendNotification);
        return label;
    }
    case IntervalListHeader::Columns::Delete:
        if (definition.intervalCents.size() == 1)
        {
            delete existingComponentToUpdate;
            existingComponentToUpdate = nullptr;
        }
        break;
    }

    return existingComponentToUpdate;
}

