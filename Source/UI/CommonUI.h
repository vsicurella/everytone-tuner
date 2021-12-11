/*
  ==============================================================================

    CommonUI.h
    Created: 13 Jul 2020 18:20:57pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
//#include "UnitGrid.h"
//#include "../TuningDefinition.h"
//#include "../TuneUpModes.h"

// Translated Text

static juce::String nameTrans = TRANS("Name");
static juce::String sizeTrans = TRANS("Size");
static juce::String periodTrans = TRANS("Period");
static juce::String generatorTrans = TRANS("Generator");
static juce::String amountTrans = TRANS("Amount");
static juce::String offsetTrans = TRANS("Offset");
static juce::String rangeTrans = TRANS("Range");
static juce::String centerTrans = TRANS("Center");
static juce::String standardTrans = TRANS("Standard") + " (12EDO)";
static juce::String currentTrans = TRANS("Current Tuning");
static juce::String browseTrans = TRANS("Browse") + " ...";
static juce::String reuseTrans = TRANS("Reuse channels when possible");
static juce::String resetTrans = TRANS("Reset empty channel pitchbend");

//// UI Helpers
//
//static int borderGap = 8;
//static int componentGap = 8;
//static int valueLabelWidth = 32;
//
//// TableListBox Helpers
//
//class TableComponent
//{
//	int rowNumber;
//	int columnId;
//
//public:
//
//	void setRowNumber(int rowNumberIn)
//	{
//		rowNumber = rowNumberIn;
//	}
//
//	void setColumnId(int columnIdIn)
//	{
//		columnId = columnIdIn;
//	}
//
//	int getRowNumber() const
//	{
//		return rowNumber;
//	}
//
//	int getColumnId() const
//	{
//		return columnId;
//	}
//};
//
//class TableLabel : public TableComponent, public juce::Label {};
//class TableButton : public TableComponent, public juce::TextButton {};
//class TableSlider : public TableComponent, public juce::Slider {};

namespace Multimapper
{
	enum Commands
	{
		Back = 1,
		Save,
		NewTuning,
		LoadTuning,
	};
}