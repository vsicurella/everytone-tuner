/*
  ==============================================================================

    TuningTableViewer.cpp
    Created: 29 Dec 2021 7:38:21pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TuningTableViewer.h"

TuningTableViewer::TuningTableViewer(IntervalListModel* intervalListModel, const MappedTuning* tuningIn)
    : juce::TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtTop)
{
    if (intervalListModel == nullptr)
    {
        intervalModel = std::make_unique<IntervalListModel>(false);
        intervalListModel = intervalModel.get();
    }

    intervalTable = std::make_unique<juce::TableListBox>("IntervalTable", intervalListModel);
    intervalTable->setHeader(std::make_unique<IntervalListHeader>(false));
    
    tuningModel = std::make_unique<TuningTableViewerModel>();
    tuningTable = std::make_unique<juce::TableListBox>("TuningTable", tuningModel.get());
    tuningTable->setHeader(std::make_unique<TuningTableHeader>());

    mappingModel = std::make_unique<MappingTableModel>();
    mappingTable = std::make_unique<juce::TableListBox>("MappingTable", mappingModel.get());
    mappingTable->setHeader(std::make_unique<MappingTableHeader>());

    addTab("Intervals", juce::Colour(), intervalTable.get(), false);
    addTab("Tuning Table", juce::Colour(), tuningTable.get(), false);
    addTab("Mapping", juce::Colour(), mappingTable.get(), false);

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

void TuningTableViewer::set(const MappedTuning* tuningIn)
{
    if (tuningIn == nullptr)
        return;

    set(tuningIn->getTuning());
    set(tuningIn->getMapping());
}

void TuningTableViewer::set(const Tuning* tuningIn)
{
    tuning = tuningIn;

    if (intervalModel != nullptr)
        intervalModel->setTuning(tuningIn);

    tuningModel->setTuning(tuningIn);
    
    intervalTable->updateContent();
    tuningTable->updateContent();
}

void TuningTableViewer::set(const TuningTableMap* mappingIn)
{
    mapping = mappingIn;
    mappingModel->setMapping(mapping);
    mappingTable->updateContent();
}

