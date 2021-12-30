/*
  ==============================================================================

    TuningTableViewer.h
    Created: 29 Dec 2021 7:38:21pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "./TuningTableViewerModel.h"
#include "./ListEditor.h"

class TuningTableViewer : public juce::TabbedComponent
{
    const Tuning* tuning;

    std::unique_ptr<ListEditor> intervalModel;
    std::unique_ptr<juce::TableListBox> intervalTable;

    std::unique_ptr<TuningTableViewerModel> tuningModel;
    std::unique_ptr<juce::TableListBox> tuningTable;

public:

    // If passed in ListEditor is nullptr, it will be created and managed by this component
    TuningTableViewer(ListEditor* listEditorModel = nullptr, const Tuning* tuningIn = nullptr);

    ~TuningTableViewer();


    void setTuning(const Tuning* tuningIn);
};
