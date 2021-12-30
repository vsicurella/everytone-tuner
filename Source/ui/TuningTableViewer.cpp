/*
  ==============================================================================

    TuningTableViewer.cpp
    Created: 29 Dec 2021 7:38:21pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TuningTableViewer.h"

TuningTableViewer::TuningTableViewer(ListEditor* listEditorModel, const Tuning* tuningIn)
    : juce::TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtTop)
{
    if (listEditorModel == nullptr)
    {
        intervalModel = std::make_unique<ListEditor>(false);
        listEditorModel = intervalModel.get();
    }

    intervalTable = std::make_unique<juce::TableListBox>("IntervalTable", listEditorModel);
    intervalTable->setHeader(std::make_unique<ListEditorHeader>(false));
    
    tuningModel = std::make_unique<TuningTableViewerModel>();
    tuningTable = std::make_unique<juce::TableListBox>("TuningTable", tuningModel.get());
    tuningTable->setHeader(std::make_unique<TuningTableHeader>());

    setTuning(tuningIn);

    addTab("Intervals", juce::Colour(), intervalTable.get(), false);
    addTab("Tuning Table", juce::Colour(), tuningTable.get(), false);
}

TuningTableViewer::~TuningTableViewer()
{
    tuningTable = nullptr;
    intervalTable = nullptr;
    intervalModel = nullptr;
}

void TuningTableViewer::setTuning(const Tuning* tuningIn)
{
    tuning = tuningIn;

    if (intervalModel != nullptr)
        intervalModel->setTuning(tuningIn);

    tuningModel->setTuning(tuningIn);
    
    intervalTable->updateContent();
    tuningTable->updateContent();
}
