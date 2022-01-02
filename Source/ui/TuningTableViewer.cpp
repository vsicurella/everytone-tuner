/*
  ==============================================================================

    TuningTableViewer.cpp
    Created: 29 Dec 2021 7:38:21pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TuningTableViewer.h"

TuningTableViewer::TuningTableViewer(IntervalListModel* intervalListModel, const MappedTuningTable* tuningIn)
    : juce::TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtTop)
{
    descriptionBox = std::make_unique<juce::TextEditor>("descriptionBox");
    descriptionBox->setMultiLine(true);
    descriptionBox->setReadOnly(true);
    descriptionBox->setScrollbarsShown(true);
    descriptionBox->setPopupMenuEnabled(true);
    addTab("Description", juce::Colour(), descriptionBox.get(), false, (int)Tabs::Description);

    set(tuningIn);
}

TuningTableViewer::~TuningTableViewer()
{
    mappingTable = nullptr;
    mappingModel = nullptr;
    tuningTable = nullptr;
    intervalTable = nullptr;
    intervalModel = nullptr;
}

void TuningTableViewer::set(const MappedTuningTable* tuningIn)
{
    if (tuningIn == nullptr)
        return;

    set(tuningIn->getTuning());
    set(tuningIn->getMapping());
}

void TuningTableViewer::set(const TuningTable* tuningIn)
{
    tuning = tuningIn;

    if (addIntervalTab())
    {
        intervalTable->updateContent();
    }

    addTuningTableTabs();
    tuningModel->setTuning(tuningIn);
    tuningTable->updateContent();
    descriptionBox->setText(tuning->getDescription());
}

void TuningTableViewer::set(const TuningTableMap* mappingIn)
{
    mapping = mappingIn;
    mappingModel->setMapping(mapping);
    mappingTable->updateContent();
}

void TuningTableViewer::addTuningTableTabs()
{
    if (tuningModel == nullptr)
    {
        tuningModel = std::make_unique<TuningTableViewerModel>();
        tuningTable = std::make_unique<juce::TableListBox>("TuningTable", tuningModel.get());
        tuningTable->setHeader(std::make_unique<TuningTableHeader>());
        addTab("Tuning Table", juce::Colour(), tuningTable.get(), false, (int)Tabs::TuningTable);
    }

    if (mappingModel == nullptr)
    {
        mappingModel = std::make_unique<MappingTableModel>();
        mappingTable = std::make_unique<juce::TableListBox>("MappingTable", mappingModel.get());
        mappingTable->setHeader(std::make_unique<MappingTableHeader>());
        addTab("Mapping", juce::Colour(), mappingTable.get(), false, (int)Tabs::Mapping);
    }
}

bool TuningTableViewer::addIntervalTab()
{
    if (getTabNames()[(int)Tabs::Intervals] == "Intervals")
    {
        removeTab((int)Tabs::Intervals);
        intervalModel = nullptr;
        //setCurrentTabIndex(0);
    }

    auto functional = dynamic_cast<const FunctionalTuning*>(tuning);
    if (functional != nullptr)
    {
        intervalModel = std::make_unique<IntervalListModel>(false, functional);
        intervalTable = std::make_unique<juce::TableListBox>("IntervalTable", intervalModel.get());
        intervalTable->setHeader(std::make_unique<IntervalListHeader>(false));

        addTab("Intervals", juce::Colour(), intervalTable.get(), false, -1);
        moveTab(getNumTabs() - 1, (int)Tabs::Intervals, false);       
        return true;
    }

    return false;
}
