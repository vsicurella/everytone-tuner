/*
  ==============================================================================

  OverviewPanel.cpp

  Created by Vincenzo Sicurella on 6/30/2020

  ==============================================================================
*/

#include "OverviewPanel.h"


//==============================================================================
OverviewPanel::OverviewPanel ()
{
	tuningNameBox.reset(new juce::Label("tuningNameBox"));
	tuningNameBox->setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(tuningNameBox.get());

	auto tuningNameLabel = labels.add(new juce::Label("tuningNameLabel", nameTrans + ":"));
	tuningNameLabel->setJustificationType(juce::Justification::centredRight);
	addAndMakeVisible(tuningNameLabel);
	tuningNameLabel->attachToComponent(tuningNameBox.get(), true);

	tuningSizeBox.reset(new juce::Label("tuningSizeBox"));
	tuningSizeBox->setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(tuningSizeBox.get());

	auto tuningSizeLabel = labels.add(new juce::Label("tuningSizeLabel", sizeTrans + ":"));
	tuningSizeLabel->setJustificationType(juce::Justification::centredRight);
	addAndMakeVisible(tuningSizeLabel);
	tuningSizeLabel->attachToComponent(tuningSizeBox.get(), true);

	tuningPeriodBox.reset(new juce::Label("tuningPeriodBox"));
	tuningPeriodBox->setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(tuningPeriodBox.get());

	auto tuningPeriodLabel = labels.add(new juce::Label("tuningPeriodLabel", periodTrans + ":"));
	tuningPeriodLabel->setJustificationType(juce::Justification::centredRight);
	addAndMakeVisible(tuningPeriodLabel);
	tuningPeriodLabel->attachToComponent(tuningPeriodBox.get(), true);

    descriptionEditor.reset (new juce::TextEditor ("descriptionEditor"));
    addAndMakeVisible (descriptionEditor.get());
    descriptionEditor->setMultiLine (true);
    descriptionEditor->setReadOnly (true);
    descriptionEditor->setScrollbarsShown (true);
    descriptionEditor->setPopupMenuEnabled (true);
	//descriptionTextBox->setTextToShowWhenEmpty(TRANS("No description"), juce::Colours::darkgrey);

	//auto descriptionlabel = labels.add(new juce::Label("descriptionLabel", juce::translate("Description") + ": "));
	//addAndMakeVisible(tuningPeriodLabel);
	//descriptionlabel->attachToComponent(descriptionTextBox.get(), false);

}

OverviewPanel::~OverviewPanel()
{
	labels.clear();
	tuningNameBox = nullptr;
	tuningSizeBox = nullptr;
	tuningPeriodBox = nullptr;
    descriptionEditor = nullptr;
}

//==============================================================================
void OverviewPanel::paint (juce::Graphics& g)
{
#if JUCE_DEBUG
	g.setColour(juce::Colours::blue);
	g.drawRect(0, 0, getWidth(), getHeight());
#endif
}

void OverviewPanel::resized()
{
	int w = getWidth();
	int h = getHeight();
	int halfWidth = juce::roundToInt(w * 0.5);
	int eighthWidth = juce::roundToInt(w * 0.125);
	int controlHeight = juce::roundToInt(h * 0.18);

	int yMargin = juce::roundToInt(h * 0.2);
	int labelWidth = w * 0.1;

	juce::FlexBox main;

	//juce::FlexBox rightHalf;

	juce::FlexBox overview;
	overview.flexDirection = juce::FlexBox::Direction::column;
	overview.justifyContent = juce::FlexBox::JustifyContent::flexStart;

	juce::FlexItem::Margin controlsMargin(0, 0, 0, labelWidth);
	overview.items.add(juce::FlexItem(halfWidth, controlHeight, *tuningNameBox).withMargin(controlsMargin));
	overview.items.add(juce::FlexItem(halfWidth, controlHeight, *tuningSizeBox).withMargin(controlsMargin));
	overview.items.add(juce::FlexItem(halfWidth, controlHeight, *tuningPeriodBox).withMargin(controlsMargin));
	overview.items.add(juce::FlexItem(halfWidth, h * 0.4, *descriptionEditor));
	//juce::FlexBox leftHalf;
	//leftHalf.flexDirection = juce::FlexBox::Direction::column;
	//leftHalf.items.add(juce::FlexItem(halfWidth, quarterHeight, overview).withMargin(juce::FlexItem::Margin(0, 0, 0, labelWidth)));

	//main.items.add(juce::FlexItem(halfWidth, h, leftHalf));
	//main.items.add(juce::FlexItem(halfWidth, h, rightHalf));

	auto overviewItem = juce::FlexItem(w, h, overview).withMargin(juce::FlexItem::Margin(0, 0, 0, 0));
	main.items.add(overviewItem);

	for (auto label : labels)
	{
		label->setSize(labelWidth, controlHeight);
	}

	main.performLayout(getLocalBounds());

	descriptionEditor->setBounds(descriptionEditor->getBounds().withRight(halfWidth).withBottom(h));
}


void OverviewPanel::setTuningDisplayed(const Tuning& tuning)
{
	setTuningNameLabel(tuning.getName());
	setDescriptionText(tuning.getDescription());
	setTuningSizeLabel(juce::String(tuning.getTuningSize()));

	//double period = ;
	//if (tuningOutDefinition[TuneUpIDs::functionalId])
	//{
	//	period = tuningOutDefinition.getChildWithName(TuneUpIDs::generatorListId).getChild(0)[TuneUpIDs::generatorValueId];
	//}
	//else
	//{
		//juce::ValueTree intervalList = tuningOutDefinition.getChild(0);
		//period = intervalList.getChild(intervalList.getNumChildren() - 1)[TuneUpIDs::intervalValueId];
	//}

	//juce::String periodDisplay = juce::String(period) + " cents";
	//double virtualPeriod = tuningOutDefinition[TuneUpIDs::virtualPeriodId];

	//if (period != virtualPeriod)
	//	periodDisplay += " (" + juce::String(virtualPeriod) + ")";
	
	setTuningPeriodLabel(juce::String(tuning.getPeriodCents()) + " cents");
}

void OverviewPanel::setTuningNameLabel(juce::String nameIn)
{
	tuningNameBox->setText(nameIn, juce::NotificationType::dontSendNotification);
}

void OverviewPanel::setTuningSizeLabel(juce::String numNotesIn)
{
	tuningSizeBox->setText(numNotesIn, juce::NotificationType::dontSendNotification);
}

void OverviewPanel::setTuningPeriodLabel(juce::String periodIn)
{
	tuningPeriodBox->setText(periodIn, juce::NotificationType::dontSendNotification);
}

void OverviewPanel::setDescriptionText(juce::String descIn)
{
	descriptionEditor->setText(descIn);
}

void OverviewPanel::tuningChanged(TuningChanger* changer, Tuning* tuning)
{
	setTuningDisplayed(*tuning);
}