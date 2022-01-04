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


    bendModeBox = std::make_unique<juce::ComboBox>("bendModeBox");
    bendModeBox->addItem("Static", (int)Everytone::BendMode::Static);
    bendModeBox->addItem("Persistent", (int)Everytone::BendMode::Persistent);
    bendModeBox->setSelectedId((int)options.bendMode);
    bendModeBox->onChange = [&]() { optionsWatchers.call(&OptionsWatcher::bendModeChanged, Everytone::BendMode(bendModeBox->getSelectedId())); };
    addAndMakeVisible(*bendModeBox);

    auto bendModeLabel = labels.add(new juce::Label("BendModeLabel", "Pitchbend Mode:"));
    bendModeLabel->attachToComponent(bendModeBox.get(), false);
    addAndMakeVisible(*bendModeLabel);


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

    voiceLimitLabel = labels.add(new juce::Label("VoiceLimitLabel", "Voice Limit:"));
    voiceLimitLabel->setJustificationType(juce::Justification::centredRight);
    voiceLimitLabel->attachToComponent(voiceLimitValueLabel.get(), true);
    addAndMakeVisible(*voiceLimitLabel);


    pitchbendRangeValue = std::make_unique<juce::Label>("pitchbendRangeValue");
    pitchbendRangeValue->setEditable(false, true);
    addAndMakeVisible(*pitchbendRangeValue);
    pitchbendRangeValue->onEditorShow = [&]()
    {
        auto editor = pitchbendRangeValue->getCurrentTextEditor();
        if (editor)
        {
            auto pitchbendText = pitchbendRangeValue->getText();
            auto rangeString = juce::StringArray::fromTokens(pitchbendText, false)[1];
            auto range = rangeString.getDoubleValue() * 2;
            editor->setText(juce::String(range), juce::NotificationType::dontSendNotification);
        }
    };
    pitchbendRangeValue->onTextChange = [&]()
    {
        auto newRange = pitchbendRangeValue->getText().getIntValue();
        optionsWatchers.call(&OptionsWatcher::pitchbendRangeChanged, newRange);
    };
    setPitchbendRangeText(options.pitchbendRange);

    pitchbendRangeLabel = labels.add(new juce::Label("pitchbendLabel", "Pitchbend Range:"));
    pitchbendRangeLabel->attachToComponent(pitchbendRangeValue.get(), true);
    addAndMakeVisible(pitchbendRangeLabel);
}

OptionsPanel::~OptionsPanel()
{
    labels.clear();
    pitchbendRangeValue = nullptr;
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
    
    float margin = 0.015f;
    margin *= (w > h) ? w : h;
        
    auto halfWidth = w * 0.5;
    
    auto controlWidth = (w - margin * 4) * 0.5f;
    auto controlHeight = h * 0.125;
    auto labelHeight = controlHeight * 0.75;

    for (auto label : labels)
        label->setSize(controlWidth, labelHeight);
    
    auto controlMargin = juce::FlexItem::Margin(margin);
    controlMargin.top += labelHeight;

    juce::FlexBox leftHalf;
    leftHalf.flexDirection = juce::FlexBox::Direction::column;
    leftHalf.items.add(juce::FlexItem(controlWidth, controlHeight, *channelModeBox).withMargin(controlMargin));
    //leftHalf.items.add(juce::FlexItem(controlWidth, controlHeight, *channelRulesBox).withMargin(controlMargin));
    leftHalf.items.add(juce::FlexItem(controlWidth, controlHeight, *bendModeBox).withMargin(controlMargin));

    juce::FlexBox rightHalf;
    rightHalf.flexDirection = juce::FlexBox::Direction::column;
    rightHalf.alignItems = juce::FlexBox::AlignItems::flexStart;
    rightHalf.items.add(juce::FlexItem(controlWidth, controlHeight, *mpeZoneBox).withMargin(controlMargin));

    auto voiceLimitWidth = voiceLimitValueLabel->getFont().getStringWidth("999999") + margin;
    auto voiceLimitLabelWidth = voiceLimitLabel->getFont().getStringWidth(voiceLimitLabel->getText());
    auto voiceLimitItemMargin = controlMargin;
    voiceLimitItemMargin.left = voiceLimitLabelWidth + margin;
    rightHalf.items.add(juce::FlexItem(voiceLimitWidth, controlHeight, *voiceLimitValueLabel).withMargin(voiceLimitItemMargin));


    auto pitchbendWidth = pitchbendRangeValue->getFont().getStringWidth(pitchbendRangeValue->getText()) + margin;
    auto pitchbendLabelWidth = pitchbendRangeLabel->getFont().getStringWidth(pitchbendRangeLabel->getText());
    auto pitchbendItemMargin = controlMargin;
    pitchbendItemMargin.left = pitchbendLabelWidth + margin;
    rightHalf.items.add(juce::FlexItem(pitchbendWidth, controlHeight, *pitchbendRangeValue).withMargin(pitchbendItemMargin));


    auto layoutMargin = juce::FlexItem::Margin(0, margin, 0, margin);

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.alignContent = juce::FlexBox::AlignContent::center;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    flexBox.items.add(juce::FlexItem(halfWidth, h, leftHalf).withMargin(layoutMargin));
    flexBox.items.add(juce::FlexItem(halfWidth, h, rightHalf).withMargin(layoutMargin));

    flexBox.performLayout(getLocalBounds());
}

void OptionsPanel::setPitchbendRangeText(int pitchbendRange)
{
    auto value = "+/- " + juce::String(pitchbendRange / 2) + " semitones";
    pitchbendRangeValue->setText(value, juce::NotificationType::dontSendNotification);
}
