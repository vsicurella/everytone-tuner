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


	rootMidiChannelBox.reset(new juce::Label("rootMidiChannelBox", "1"));
	addAndMakeVisible(rootMidiChannelBox.get());
	rootMidiChannelBox->setEditable(false, true);
	rootMidiChannelBox->onEditorShow = [&]() { rootChannelBackup = rootMidiChannelBox->getText(); };
	rootMidiChannelBox->onTextChange = [&]() { tuningReferenceEdited(); };

	auto rootChannelLabel = labels.add(new juce::Label("rootChannelLabel", "Root MIDI Channel:"));
	rootChannelLabel->setJustificationType(juce::Justification::centredRight);
	rootChannelLabel->attachToComponent(rootMidiChannelBox.get(), true);
	addAndMakeVisible(rootChannelLabel);

	rootMidiNoteBox.reset(new juce::Label("rootMidiNoteBox", "60"));
	addAndMakeVisible(rootMidiNoteBox.get());
	rootMidiNoteBox->setEditable(false, true);
	rootMidiNoteBox->onEditorShow = [&]() { rootNoteBackup = rootMidiNoteBox->getText(); };
	rootMidiNoteBox->onTextChange = [&]() { tuningReferenceEdited(); };

	auto rootNoteLabel = labels.add(new juce::Label("rootNoteLabel", "Root MIDI Note:"));
	rootNoteLabel->setJustificationType(juce::Justification::centredRight);
	rootNoteLabel->attachToComponent(rootMidiNoteBox.get(), true);
	addAndMakeVisible(rootNoteLabel);

	rootFrequencyBox.reset(new juce::Label("rootFrequencyBox", "263"));
	addAndMakeVisible(rootFrequencyBox.get());
	rootFrequencyBox->setEditable(false, true);
	rootFrequencyBox->onEditorShow = [&]() { rootFrequencyBackup = rootFrequencyBox->getText(); };
	rootFrequencyBox->onTextChange = [&]() { tuningReferenceEdited(); };

	auto rootFrequencyLabel = labels.add(new juce::Label("rootFrequencyLabel", "Root Frequency:"));
	rootFrequencyLabel->setJustificationType(juce::Justification::centredRight);
	rootFrequencyLabel->attachToComponent(rootFrequencyBox.get(), true);
	addAndMakeVisible(rootFrequencyLabel);

	auto mappingButtonCallback = [&]() { mappingTypeButtonClicked(); };

	linearMappingButton.reset(new juce::TextButton("linearMappingButton"));
	addAndMakeVisible(linearMappingButton.get());
	linearMappingButton->setButtonText("Linear");
	linearMappingButton->setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
	linearMappingButton->setClickingTogglesState(true);
	linearMappingButton->setToggleState(options.mappingType == Everytone::MappingType::Linear, juce::NotificationType::dontSendNotification);
	linearMappingButton->setRadioGroupId(10);
	linearMappingButton->onClick = mappingButtonCallback;

	periodicMappingButton.reset(new juce::TextButton("periodicMappingButton"));
	addAndMakeVisible(periodicMappingButton.get());
	periodicMappingButton->setButtonText("Periodic");
	periodicMappingButton->setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft);
	periodicMappingButton->setClickingTogglesState(true);
	periodicMappingButton->setToggleState(options.mappingType == Everytone::MappingType::Periodic, juce::NotificationType::dontSendNotification);
	periodicMappingButton->setRadioGroupId(10);

	auto mappingLabel = labels.add(new juce::Label("mappingLabel", "Mapping:"));
	mappingLabel->setJustificationType(juce::Justification::centredRight);
	mappingLabel->attachToComponent(linearMappingButton.get(), true);
	addAndMakeVisible(mappingLabel);

	pitchbendRangeValue = std::make_unique<juce::Label>("pitchbendRangeValue");
	pitchbendRangeValue->setEditable(false, true);
	addAndMakeVisible(*pitchbendRangeValue);
	pitchbendRangeValue->onEditorShow = [&]()
	{
		auto editor = pitchbendRangeValue->getCurrentTextEditor();
		if (editor)
		{
			auto pitchbendText = pitchbendRangeValue->getText();
			auto range = juce::StringArray::fromTokens(pitchbendText, false)[1];
			editor->setText(range, juce::NotificationType::dontSendNotification);
		}
	};
	pitchbendRangeValue->onTextChange = [&]() 
	{ 
		auto newRange = pitchbendRangeValue->getText().getIntValue();
		optionsWatchers.call(&OptionsWatcher::pitchbendRangeChanged, newRange); 
		setPitchbendRangeText(newRange);
	};
	setPitchbendRangeText(options.pitchbendRange / 2);

	auto pitchbendLabel = labels.add(new juce::Label("pitchbendLabel", "Pitchbend Range:"));
	pitchbendLabel->attachToComponent(pitchbendRangeValue.get(), true);
	addAndMakeVisible(pitchbendLabel);
}

OverviewPanel::~OverviewPanel()
{
	labels.clear();

	periodicMappingButton = nullptr;
	linearMappingButton = nullptr;
	rootFrequencyBox = nullptr;
	rootMidiNoteBox = nullptr;
	rootMidiChannelBox = nullptr;

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
	tuningInfo.flexDirection = juce::FlexBox::Direction::column;

	juce::FlexItem::Margin tuningMargin(0, 0, 0, tuningLabelWidth);
	tuningInfo.items.add(juce::FlexItem(halfWidth, controlHeight, *tuningNameBox).withMargin(tuningMargin));
	tuningInfo.items.add(juce::FlexItem(halfWidth, controlHeight, *tuningSizeBox).withMargin(tuningMargin));
	tuningInfo.items.add(juce::FlexItem(halfWidth, controlHeight, *tuningPeriodBox).withMargin(tuningMargin));
	tuningInfo.items.add(juce::FlexItem(halfWidth, h * 0.4, *descriptionEditor));
	
	
	juce::FlexBox referenceInfo;
	referenceInfo.flexDirection = juce::FlexBox::Direction::column;

	int referenceControlWidth = w * 0.18;
	int referenceWidth = w * 0.38;
	int referenceLabelWidth = w * 0.15;
	juce::FlexItem::Margin referenceMargin(0, 0, 0, referenceLabelWidth);
	
	referenceInfo.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *rootMidiChannelBox).withMargin(referenceMargin));
	referenceInfo.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *rootMidiNoteBox).withMargin(referenceMargin));
	referenceInfo.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *rootFrequencyBox).withMargin(referenceMargin));

	int buttonWidth = juce::roundToInt(w * 0.15);
	int buttonHeight = juce::roundToInt(controlHeight * 0.6);

	juce::FlexBox mappingRow;
	mappingRow.justifyContent = juce::FlexBox::JustifyContent::flexStart;
	mappingRow.items.add(juce::FlexItem(buttonWidth, buttonHeight, *linearMappingButton));
	mappingRow.items.add(juce::FlexItem(buttonWidth, buttonHeight, *periodicMappingButton));

	referenceInfo.items.add(juce::FlexItem(buttonWidth * 2, buttonHeight, mappingRow).withMargin(referenceMargin));
	referenceInfo.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *pitchbendRangeValue).withMargin(referenceMargin));

	juce::FlexBox main;
	main.justifyContent = juce::FlexBox::JustifyContent::flexStart;

	auto tuningItem = juce::FlexItem(halfWidth, h, tuningInfo);
	main.items.add(tuningItem);

	auto referenceItem = juce::FlexItem(referenceWidth, h, referenceInfo)
									   .withMargin(juce::FlexItem::Margin(0, 0, 0, 10));
	main.items.add(referenceItem);

	for (auto label : labels)
	{
		label->setSize(tuningLabelWidth, controlHeight);
	}

	main.performLayout(getLocalBounds());

	descriptionEditor->setBounds(descriptionEditor->getBounds().withRight(halfWidth).withBottom(h));

#if JUCE_DEBUG
	debugBoxes.clear();
	/*debugBoxes.add(getLocalBounds());
	for (auto box : main.items)
		debugBoxes.add(box.currentBounds.toNearestInt().reduced(2));	*/
#endif
}


void OverviewPanel::setTuningDisplayed(const MappedTuning* mappedTuning)
{
	auto tuning = mappedTuning->getTuning();
	setTuningNameLabel(tuning->getName());
	setDescriptionText(tuning->getDescription());

	auto periodLabel = juce::String(tuning->getPeriodCents());
	auto virtualPeriod = tuning->getVirtualPeriod();
	if (virtualPeriod != 0)
		periodLabel += " (" + juce::String(virtualPeriod) + ")";
	setTuningPeriodLabel(periodLabel);

	auto sizeLabel = juce::String(tuning->getTuningSize());
	auto virtualSize = tuning->getVirtualSize();
	if (virtualSize != 0)
		sizeLabel += " (" + juce::String(virtualSize) + ")";
	setTuningSizeLabel(sizeLabel);

	auto mapping = mappedTuning->getMapping();
	setRootMidiChannelLabel(juce::String(mapping->getRootMidiChannel()));
	setRootMidiNoteLabel(juce::String(mapping->getRootMidiNote()));
	
	setRootFrequencyLabel(juce::String(tuning->getRootFrequency()) + " hz");
}

void OverviewPanel::mappingTypeButtonClicked()
{
	auto type = (linearMappingButton->getToggleState())
		? Everytone::MappingType::Linear
		: Everytone::MappingType::Periodic;

	optionsWatchers.call(&OptionsWatcher::mappingTypeChanged, type);
}

void OverviewPanel::tuningReferenceEdited()
{
	MappedTuning::Root newRoot;

	auto channelInput = rootMidiChannelBox->getText().trim();
	int channelValue = -1;
	if (channelInput.containsOnly("0123456789"))
	{
		channelValue = channelInput.getIntValue();
		if (channelValue >= 1 && channelValue <= 16)
			newRoot.midiChannel = channelValue;
	}
	if (channelValue < 0)
	{
		rootMidiChannelBox->setText(rootChannelBackup, juce::NotificationType::dontSendNotification);
	}

	auto noteInput = rootMidiNoteBox->getText().trim();
	int noteValue = -1;
	if (noteInput.containsOnly("0123456789"))
	{
		noteValue = noteInput.getIntValue();
		if (noteValue >= 0 && noteValue < 128)
			newRoot.midiNote = noteValue;
	}
	if (noteValue < 0)
	{
		rootMidiNoteBox->setText(rootNoteBackup, juce::NotificationType::dontSendNotification);
	}

	auto freqInput = rootFrequencyBox->getText().trim();
	if (freqInput.endsWith("hz"))
		freqInput = freqInput.substring(0, freqInput.length() - 3).trim();

	double freqValue = -1;
	if (freqInput.containsOnly("0123456789."))
	{
		freqValue = freqInput.getDoubleValue();
		if (freqValue >= 8.0 && freqValue < 14000.0)
			newRoot.frequency = freqValue;

		rootFrequencyBox->setText(freqInput + " hz", juce::NotificationType::dontSendNotification);
	}
	if (freqValue < 0)
	{
		rootFrequencyBox->setText(rootFrequencyBackup, juce::NotificationType::dontSendNotification);
	}

	tuningWatchers.call(&TuningWatcher::targetMappedTuningRootChanged, this, newRoot);
}

void OverviewPanel::setPitchbendRangeText(int pitchbendRange)
{
	auto value = "+/- " + juce::String(pitchbendRange) + " semitones";
	pitchbendRangeValue->setText(value, juce::NotificationType::dontSendNotification);
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

void OverviewPanel::setRootMidiChannelLabel(juce::String channel)
{
	rootMidiChannelBox->setText(channel, juce::NotificationType::dontSendNotification);
}
void OverviewPanel::setRootMidiNoteLabel(juce::String note)
{
	rootMidiNoteBox->setText(note, juce::NotificationType::dontSendNotification);
}
void OverviewPanel::setRootFrequencyLabel(juce::String frequency)
{
	rootFrequencyBox->setText(frequency, juce::NotificationType::dontSendNotification);
}

//void OverviewPanel::tuningChanged(TuningChanger* changer, Tuning* tuning)
//{
//	setTuningDisplayed(*tuning);
//}