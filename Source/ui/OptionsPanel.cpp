/*
  ==============================================================================

    OptionsPanel.cpp
    Created: 19 Dec 2021 4:54:19pm
    Author:  Vincenzo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OptionsPanel.h"

//==============================================================================
OptionsPanel::OptionsPanel(Everytone::Options options)
{
    channelModeBox = std::make_unique<juce::ComboBox>("ChannelModeBox");
    channelModeBox->addItem("First Available", (int)Everytone::ChannelMode::FirstAvailable);
    channelModeBox->addItem("Round Robin", (int)Everytone::ChannelMode::RoundRobin);
    channelModeBox->setSelectedId((int)options.channelMode, juce::NotificationType::dontSendNotification);
    channelModeBox->onChange = [&]() 
    { 
        optionsWatchers.call(&OptionsWatcher::channelModeChanged, Everytone::ChannelMode(channelModeBox->getSelectedId())); 
    };
    addAndMakeVisible(*channelModeBox);

    auto channelModeBoxLabel = labels.add(new juce::Label("ChannelModeLabel", "Channel Mode:"));
    channelModeBoxLabel->attachToComponent(channelModeBox.get(), false);
    addAndMakeVisible(*channelModeBoxLabel);


    channelRulesBox = std::make_unique<juce::ComboBox>("ChannelRulesBox");
    channelRulesBox->addItem("One note per channel", (int)Everytone::MidiMode::Mono);
    channelRulesBox->addItem("Poly channels if pitchbend match", (int)Everytone::MidiMode::Poly);
    channelRulesBox->setSelectedId((int)options.midiMode, juce::NotificationType::dontSendNotification);
    channelRulesBox->onChange = [&]()
    {
        optionsWatchers.call(&OptionsWatcher::midiModeChanged, Everytone::MidiMode(channelRulesBox->getSelectedId()));
    };
    addAndMakeVisible(*channelRulesBox);
    
    auto channelRulesBoxLabel = labels.add(new juce::Label("ChannelRulesLabel", "Channel Rules:"));
    channelRulesBoxLabel->attachToComponent(channelRulesBox.get(), false);
    addAndMakeVisible(*channelRulesBoxLabel);


    mpeZoneBox = std::make_unique<juce::ComboBox>("mpeZoneBox");
    mpeZoneBox->addItem("Lower", (int)Everytone::MpeZone::Lower);
    mpeZoneBox->addItem("Upper", (int)Everytone::MpeZone::Upper);
    mpeZoneBox->addItem("Omnichannel", (int)Everytone::MpeZone::Omnichannel);
    mpeZoneBox->setSelectedId((int)options.mpeZone, juce::NotificationType::dontSendNotification);
    mpeZoneBox->onChange = [&]() { optionsWatchers.call(&OptionsWatcher::mpeZoneChanged, Everytone::MpeZone(mpeZoneBox->getSelectedId())); };
    addAndMakeVisible(*mpeZoneBox);

    auto mpeZoneLabel = labels.add(new juce::Label("MpeZoneLabel", "MPE Zone:"));
    mpeZoneLabel->attachToComponent(mpeZoneBox.get(), false);
    addAndMakeVisible(*mpeZoneLabel);


    voiceLimitValueLabel = std::make_unique<juce::Label>("VoiceLimitValue");
    voiceLimitValueLabel->setEditable(true);
    voiceLimitValueLabel->setText(juce::String(options.voiceLimit), juce::NotificationType::dontSendNotification);
    voiceLimitValueLabel->onTextChange = [&]()
    {
        optionsWatchers.call(&OptionsWatcher::voiceLimitChanged, voiceLimitValueLabel->getText().getIntValue());
    };
    addAndMakeVisible(*voiceLimitValueLabel);

    auto voiceLimitLabel = labels.add(new juce::Label("VoiceLimitLabel", "Voice Limit:"));
    voiceLimitLabel->attachToComponent(voiceLimitValueLabel.get(), true);
    addAndMakeVisible(*voiceLimitLabel);
}

OptionsPanel::~OptionsPanel()
{
    labels.clear();
    voiceLimitValueLabel = nullptr;
    mpeZoneBox = nullptr;
    channelRulesBox = nullptr;
    channelModeBox = nullptr;
}

void OptionsPanel::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

}

void OptionsPanel::resized()
{
    auto w = getWidth();
    auto h = getHeight();
    
    auto halfWidth = w * 0.5;
    
    auto controlWidth = halfWidth * 0.75f;
    auto controlHeight = h * 0.125;
    auto labelHeight = controlHeight * 0.75;
    
    auto controlMargin = juce::FlexItem::Margin(labelHeight + 5, w / 15.0f, 0, 0);

    juce::FlexBox leftHalf;
    leftHalf.flexDirection = juce::FlexBox::Direction::column;
    leftHalf.items.add(juce::FlexItem(controlWidth, controlHeight, *channelModeBox).withMargin(controlMargin));
    leftHalf.items.add(juce::FlexItem(controlWidth, controlHeight, *channelRulesBox).withMargin(controlMargin));

    juce::FlexBox rightHalf;
    rightHalf.flexDirection = juce::FlexBox::Direction::column;
    rightHalf.alignItems = juce::FlexBox::AlignItems::flexEnd;
    rightHalf.items.add(juce::FlexItem(halfWidth, controlHeight, *mpeZoneBox).withMargin(controlMargin));
    rightHalf.items.add(juce::FlexItem(w * 0.1, controlHeight, *voiceLimitValueLabel).withMargin(controlMargin));

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.alignContent = juce::FlexBox::AlignContent::center;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    flexBox.items.add(juce::FlexItem(halfWidth, h, leftHalf));
    flexBox.items.add(juce::FlexItem(halfWidth, h, rightHalf));

    for (auto label : labels)
        label->setSize(controlWidth, labelHeight);

    flexBox.performLayout(getLocalBounds());
}
