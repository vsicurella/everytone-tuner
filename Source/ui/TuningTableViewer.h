/*
  ==============================================================================

    TuningTableViewer.h
    Created: 29 Dec 2021 7:38:21pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "./TuningTableViewerModel.h"
#include "./IntervalListEditor.h"
#include "./MappingTableModel.h"

class TuningTableViewer : public juce::TabbedComponent
{
    const MappedTuningTable* mappedTuning;
    const TuningTable* tuning;
    const TuningTableMap* mapping;

    std::unique_ptr<IntervalListModel> intervalModel;
    std::unique_ptr<juce::TableListBox> intervalTable;

    std::unique_ptr<TuningTableViewerModel> tuningModel;
    std::unique_ptr<juce::TableListBox> tuningTable;

    std::unique_ptr<MappingTableModel> mappingModel;
    std::unique_ptr<juce::TableListBox> mappingTable;

public:

    // If passed in IntervalListModel is nullptr, it will be created and managed by this component
    TuningTableViewer(IntervalListModel* intervalListModel = nullptr, const MappedTuningTable* tuningIn = nullptr);

    ~TuningTableViewer();

    void set(const MappedTuningTable* tuningIn);
    void set(const TuningTable* tuningIn);
    void set(const TuningTableMap* tuningIn);
};
