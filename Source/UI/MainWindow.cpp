/*
  ==============================================================================

  MainWindow.cpp

  Created by Vincenzo Sicurella on 6/30/2020

  ==============================================================================
*/

#include "MainWindow.h"


//==============================================================================
MainWindow::MainWindow ()
	//: grid(6)
{
	tuningNameLabel.reset(new juce::Label("tuningNameLabel", nameTrans + ":"));
	tuningNameLabel->setJustificationType(juce::Justification::centredRight);
	addAndMakeVisible(tuningNameLabel.get());

	tuningNameBox.reset(new juce::Label("tuningNameBox"));
	tuningNameBox->setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(tuningNameBox.get());

	tuningSizeLabel.reset(new juce::Label("tuningSizeLabel", sizeTrans + ":"));
	tuningSizeLabel->setJustificationType(juce::Justification::centredRight);
	addAndMakeVisible(tuningSizeLabel.get());

	tuningSizeBox.reset(new juce::Label("tuningSizeBox"));
	tuningSizeBox->setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(tuningSizeBox.get());

	tuningPeriodLabel.reset(new juce::Label("tuningPeriodLabel", periodTrans + ":"));
	tuningPeriodLabel->setJustificationType(juce::Justification::centredRight);
	addAndMakeVisible(tuningPeriodLabel.get());

	tuningPeriodBox.reset(new juce::Label("tuningPeriodBox"));
	tuningPeriodBox->setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(tuningPeriodBox.get());

    descriptionTextBox.reset (new juce::TextEditor ("descriptionTextBox"));
    addAndMakeVisible (descriptionTextBox.get());
    descriptionTextBox->setMultiLine (true);
    descriptionTextBox->setReadOnly (true);
    descriptionTextBox->setScrollbarsShown (true);
    descriptionTextBox->setPopupMenuEnabled (true);
	descriptionTextBox->setTextToShowWhenEmpty(TRANS("No description"), juce::Colours::darkgrey);

	for (auto str : { nameTrans, sizeTrans, periodTrans })
	{
		int width = font.getStringWidth(str + ":") + componentGap;
		if (width > valueLabelWidth)
			valueLabelWidth = width;
	}
}

MainWindow::~MainWindow()
{
	tuningNameLabel = nullptr;
	tuningNameBox = nullptr;
	tuningSizeLabel = nullptr;
	tuningSizeBox = nullptr;
	tuningPeriodLabel = nullptr;
	tuningPeriodBox = nullptr;
    descriptionTextBox = nullptr;
}

//==============================================================================
void MainWindow::paint (juce::Graphics& g)
{
	//g.setColour(Colours::blue);
	//g.drawRect(0, 0, getWidth(), getHeight());
}

void MainWindow::resized()
{
	int w = getWidth();
	int h = getHeight();
	double halfWidth = w * 0.5;
	double eighthWidth = w * 0.125;
	double thirdHeight = h * 0.333333;

	juce::FlexBox main;

	juce::FlexBox rightHalf;
	rightHalf.items.add(juce::FlexItem(halfWidth, h, *descriptionTextBox));
	//descriptionTextBox->setBounds(proportionOfWidth(0.5f), 0, proportionOfWidth(0.5f), getHeight());

	juce::FlexBox nameRow;
	nameRow.items.add(juce::FlexItem(eighthWidth, thirdHeight, *tuningNameLabel));
	nameRow.items.add(juce::FlexItem(halfWidth, thirdHeight, *tuningNameBox));
	//tuningNameLabel->setBounds(0, 0, valueLabelWidth, grid.getUnit(2) - componentGap);
	//tuningNameBox->setBounds(valueLabelWidth, tuningNameLabel->getY(), descriptionTextBox->getX() - valueLabelWidth - componentGap, tuningNameLabel->getHeight());

	juce::FlexBox sizeRow;
	sizeRow.items.add(juce::FlexItem(eighthWidth, thirdHeight, *tuningSizeLabel));
	sizeRow.items.add(juce::FlexItem(halfWidth, thirdHeight, *tuningSizeBox));
	//tuningSizeLabel->setBounds(0, grid.getUnit(2), valueLabelWidth, grid.getUnit(2) - componentGap);
	//tuningSizeBox->setBounds(valueLabelWidth, tuningSizeLabel->getY(), descriptionTextBox->getX() - valueLabelWidth - componentGap, tuningSizeLabel->getHeight());

	juce::FlexBox periodRow;
	periodRow.items.add(juce::FlexItem(eighthWidth, thirdHeight, *tuningPeriodLabel));
	periodRow.items.add(juce::FlexItem(halfWidth, thirdHeight, *tuningPeriodBox));
	//tuningPeriodLabel->setBounds(0, grid.getUnit(4), valueLabelWidth, grid.getUnit(2) - componentGap);
	//tuningPeriodBox->setBounds(valueLabelWidth, tuningPeriodLabel->getY(), descriptionTextBox->getX() - valueLabelWidth - componentGap, tuningPeriodLabel->getHeight());

	juce::FlexBox leftHalf;
	leftHalf.flexDirection = juce::FlexBox::Direction::column;
	leftHalf.items.add(juce::FlexItem(halfWidth, thirdHeight, nameRow));
	leftHalf.items.add(juce::FlexItem(halfWidth, thirdHeight, sizeRow));
	leftHalf.items.add(juce::FlexItem(halfWidth, thirdHeight, periodRow));

	main.items.add(juce::FlexItem(halfWidth, h, leftHalf));
	main.items.add(juce::FlexItem(halfWidth, h, rightHalf));

	main.performLayout(getLocalBounds());
}

void MainWindow::loadOptionsNode(juce::ValueTree optionsNodeIn)
{
	optionsNode = optionsNodeIn;
	updateTuningOutProperties();
}

void MainWindow::updateTuningOutProperties()
{
	setTuningNameLabel("");
	setDescriptionText("");
	setTuningSizeLabel("");

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
	
	setTuningPeriodLabel("");
}

void MainWindow::setTuningNameLabel(juce::String nameIn)
{
	tuningNameBox->setText(nameIn, juce::NotificationType::dontSendNotification);
}

void MainWindow::setTuningSizeLabel(juce::String numNotesIn)
{
	tuningSizeBox->setText(numNotesIn, juce::NotificationType::dontSendNotification);
}

void MainWindow::setTuningPeriodLabel(juce::String periodIn)
{
	tuningPeriodBox->setText(periodIn, juce::NotificationType::dontSendNotification);
}

void MainWindow::setDescriptionText(juce::String descIn)
{
	descriptionTextBox->setText(descIn);
}

