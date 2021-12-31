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
    //if (intervalListModel == nullptr)
    //{
    //    intervalModel = std::make_unique<IntervalListModel>(false);
    //    intervalListModel = intervalModel.get();
    //}

    //intervalTable = std::make_unique<juce::TableListBox>("IntervalTable", intervalListModel);
    //intervalTable->setHeader(std::make_unique<IntervalListHeader>(false));
    //
    //tuningModel = std::make_unique<TuningTableViewerModel>();
    //tuningTable = std::make_unique<juce::TableListBox>("TuningTable", tuningModel.get());
    //tuningTable->setHeader(std::make_unique<TuningTableHeader>());

    //mappingModel = std::make_unique<MappingTableModel>();
    //mappingTable = std::make_unique<juce::TableListBox>("MappingTable", mappingModel.get());
    //mappingTable->setHeader(std::make_unique<MappingTableHeader>());

    //addTab("Intervals", juce::Colour(), intervalTable.get(), false);
    //addTab("Tuning Table", juce::Colour(), tuningTable.get(), false);
    //addTab("Mapping", juce::Colour(), mappingTable.get(), false);

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
        addTab("Tuning Table", juce::Colour(), tuningTable.get(), false, (int)TuningTableViewerTabs::TuningTable);
    }

    if (mappingModel == nullptr)
    {
        mappingModel = std::make_unique<MappingTableModel>();
        mappingTable = std::make_unique<juce::TableListBox>("MappingTable", mappingModel.get());
        mappingTable->setHeader(std::make_unique<MappingTableHeader>());
        addTab("Mapping", juce::Colour(), mappingTable.get(), false, (int)TuningTableViewerTabs::Mapping);
    }
}

bool TuningTableViewer::addIntervalTab()
{
    auto functional = dynamic_cast<const FunctionalTuning*>(tuning);

    if (functional == nullptr)
    {
        removeTab((int)TuningTableViewerTabs::Intervals);
        intervalModel = nullptr;
        setCurrentTabIndex(0);
        return false;
    }

    if (intervalModel == nullptr)
    {
        intervalModel = std::make_unique<IntervalListModel>(false);
        intervalTable = std::make_unique<juce::TableListBox>("IntervalTable", intervalModel.get());
        intervalTable->setHeader(std::make_unique<IntervalListHeader>(false));
        addTab("Intervals", juce::Colour(), intervalTable.get(), false, (int)TuningTableViewerTabs::Intervals);

        setCurrentTabIndex(0);
    }

    intervalModel->setTuning(functional);

    return true;
}
