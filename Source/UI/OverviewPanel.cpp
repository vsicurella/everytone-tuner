/*
  ==============================================================================

  OverviewPanel.cpp

  Created by Vincenzo Sicurella on 6/30/2020

  ==============================================================================
*/

#include "OverviewPanel.h"

//==============================================================================
OverviewPanel::OverviewPanel (Everytone::Options options)
{
	tuningNameBox.reset(new juce::Label("tuningNameBox"));
	tuningNameBox->setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(tuningNameBox.get());

	auto tuningNameLabel = labels.add(new juce::Label("tuningNameLabel", juce::translate("Name") + ":"));
	tuningNameLabel->setJustificationType(juce::Justification::centredRight);
	tuningNameLabel->attachToComponent(tuningNameBox.get(), true);
	addAndMakeVisible(tuningNameLabel);

	tuningSizeBox.reset(new juce::Label("tuningSizeBox"));
	tuningSizeBox->setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(tuningSizeBox.get());

	auto tuningSizeLabel = labels.add(new juce::Label("tuningSizeLabel", juce::translate("Size") + ":"));
	tuningSizeLabel->setJustificationType(juce::Justification::centredRight);
	tuningSizeLabel->attachToComponent(tuningSizeBox.get(), true);
	addAndMakeVisible(tuningSizeLabel);

	tuningPeriodBox.reset(new juce::Label("tuningPeriodBox"));
	tuningPeriodBox->setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(tuningPeriodBox.get());

	auto tuningPeriodLabel = labels.add(new juce::Label("tuningPeriodLabel", juce::translate("Period") + ":"));
	tuningPeriodLabel->setJustificationType(juce::Justification::centredRight);
	tuningPeriodLabel->attachToComponent(tuningPeriodBox.get(), true);
	addAndMakeVisible(tuningPeriodLabel);

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

	//listEditorComponent = std::make_unique<juce::TableListBox>("ListEditorComponent");
	//listEditorComponent->setHeader(std::make_unique<IntervalListHeader>(true));
	//addAndMakeVisible(listEditorComponent.get());

	tuningTableViewer = std::make_unique<TuningTableViewer>();
	addAndMakeVisible(*tuningTableViewer);
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
	juce::Random r(420);
	for (auto rect : debugBoxes)
	{
		g.setColour(juce::Colour(r.nextFloat(), 1.0f, 1.0f, 1.0f));
		g.drawRect(rect);
	}

	/*rootMidiChannelBox->setColour(juce::Label::ColourIds::outlineColourId, juce::Colour(r.nextFloat(), 1.0f, 1.0f, 1.0f));
	rootMidiNoteBox->setColour(juce::Label::ColourIds::outlineColourId, juce::Colour(r.nextFloat(), 1.0f, 1.0f, 1.0f));
	rootFrequencyBox->setColour(juce::Label::ColourIds::outlineColourId, juce::Colour(r.nextFloat(), 1.0f, 1.0f, 1.0f));*/
#endif
}

void OverviewPanel::resized()
{
	int w = getWidth();
	int h = getHeight();
	int halfWidth = juce::roundToInt(w * 0.5);
	int controlHeight = juce::roundToInt(h * 0.18);

	int yMargin = juce::roundToInt(h * 0.2);
	int tuningLabelWidth = w * 0.1;


	juce::FlexBox tuningInfo;
	tuningInfo.flexDirection = juce::FlexBox::Direction::row;

	juce::FlexItem::Margin tuningMargin(0, 0, 0, tuningLabelWidth);
	//tuningInfo.items.add(juce::FlexItem(halfWidth, controlHeight, *tuningNameBox).withMargin(tuningMargin));
	//tuningInfo.items.add(juce::FlexItem(halfWidth, controlHeight, *tuningSizeBox).withMargin(tuningMargin));
	//tuningInfo.items.add(juce::FlexItem(halfWidth, controlHeight, *tuningPeriodBox).withMargin(tuningMargin));
	tuningInfo.items.add(juce::FlexItem(halfWidth, h, *descriptionEditor));
	
	tuningInfo.items.add(juce::FlexItem(halfWidth, h, *tuningTableViewer));
	
	//juce::FlexBox main;
	//main.justifyContent = juce::FlexBox::JustifyContent::flexStart;

	//auto tuningItem = juce::FlexItem(halfWidth, h, tuningInfo);
	//main.items.add(tuningItem);


	//for (auto label : labels)
	//{
	//	label->setSize(tuningLabelWidth, controlHeight);
	//}

	tuningInfo.performLayout(getLocalBounds());

	//descriptionEditor->setBounds(descriptionEditor->getBounds().withRight(halfWidth).withBottom(h));

#if JUCE_DEBUG
	debugBoxes.clear();
	/*debugBoxes.add(getLocalBounds());
	for (auto box : main.items)
		debugBoxes.add(box.currentBounds.toNearestInt().reduced(2));	*/
#endif
}


void OverviewPanel::setTuningDisplayed(const MappedTuningTable* mappedTuningIn)
{
	mappedTuning = mappedTuningIn;
	auto tuning = mappedTuning->getTuning();

	tuningNameBox->setText(tuning->getName(), juce::NotificationType::dontSendNotification);
	descriptionEditor->setText(tuning->getDescription(), juce::NotificationType::dontSendNotification);

	//auto periodLabel = juce::String(tuning->getPeriodCents());
	//auto virtualPeriod = tuning->getVirtualPeriod();
	//if (virtualPeriod != 0)
	//	periodLabel += " (" + juce::String(virtualPeriod) + ")";
	//tuningPeriodBox->setText(periodLabel, juce::NotificationType::dontSendNotification);

	//auto sizeLabel = juce::String(tuning->getTuningSize());
	//auto virtualSize = tuning->getVirtualSize();
	//if (virtualSize != 0)
	//	sizeLabel += " (" + juce::String(virtualSize) + ")";
	//tuningSizeBox->setText(sizeLabel, juce::NotificationType::dontSendNotification);

	//auto mapping = mappedTuning->getMapping();
	//setRootMidiChannelLabel(juce::String(mapping->getRootMidiChannel()));
	//setRootMidiNoteLabel(juce::String(mapping->getRootMidiNote()));
	//
	//setRootFrequencyLabel(juce::String(tuning->getRootFrequency()) + " hz");

	//listEditorComponent->updateContent();
	tuningTableViewer->set(mappedTuning);
}

void OverviewPanel::setListEditorModel(IntervalListModel* listEditor)
{
	//listEditorComponent->setModel(listEditor);
	tuningTableViewer = nullptr;
	tuningTableViewer = std::make_unique<TuningTableViewer>(listEditor);
	tuningTableViewer->set(mappedTuning);
	addAndMakeVisible(*tuningTableViewer);
	resized();
}

//
//void OverviewPanel::setTuningNameLabel(juce::String nameIn)
//{
//}
//
//void OverviewPanel::setTuningSizeLabel(juce::String numNotesIn)
//{
//}

//void OverviewPanel::setTuningPeriodLabel(juce::String periodIn)
//{
//	tuningPeriodBox->setText(periodIn, juce::NotificationType::dontSendNotification);
//}
