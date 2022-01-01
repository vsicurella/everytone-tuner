/*
  ==============================================================================

    InfoBar.h
    Created: 27 Dec 2021 1:33:05pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class InfoBar  : public juce::Component
{
    std::unique_ptr<juce::TextButton> actionButton;
    std::unique_ptr<juce::Label> nameValueLabel;
    //std::unique_ptr<juce::Label> sizeValueLabel;
    //std::unique_ptr<juce::Label> periodValueLabel;

    bool buttonIsBack = false;

    juce::String mappedTuningSummary;

public:
    InfoBar(juce::ApplicationCommandManager* cmdManager)
    {
        actionButton = std::make_unique<juce::TextButton>("actionButton");
        actionButton->setButtonText("=");
        actionButton->onClick = [this, cmdManager]()
        {
            if (buttonIsBack)
            {
                cmdManager->invokeDirectly(Everytone::Commands::Back, true);
                return;
            }

            cmdManager->invokeDirectly(Everytone::Commands::ShowMenu, true);
        };
        addAndMakeVisible(*actionButton);

        nameValueLabel = std::make_unique<juce::Label>("nameLabel", "");
        nameValueLabel->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*nameValueLabel);

        //sizeValueLabel = std::make_unique<juce::Label>("sizeLabel", "");
        //sizeValueLabel->setJustificationType(juce::Justification::centred);
        //addAndMakeVisible(*sizeValueLabel);

        //periodValueLabel = std::make_unique<juce::Label>("periodLabel", "");
        //periodValueLabel->setJustificationType(juce::Justification::centred);
        //addAndMakeVisible(*periodValueLabel);
    }

    ~InfoBar() override
    {
        //periodValueLabel = nullptr;
        //sizeValueLabel = nullptr;
        nameValueLabel = nullptr;
        actionButton = nullptr;
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll(juce::LookAndFeel::getDefaultLookAndFeel().findColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId));
    }

    void resized() override
    {
        auto w = getWidth();
        auto h = getHeight();

        auto buttonWidth = h * 1.5;

        auto margin = h * 0.15f;

        //auto sizeWidth = sizeValueLabel->getFont().getStringWidth(sizeValueLabel->getText()) + margin;
        //auto periodWidth = periodValueLabel->getFont().getStringWidth(periodValueLabel->getText()) + margin;

        //auto nameWidth = w - (buttonWidth + sizeWidth + periodWidth) - margin;

        auto nameWidth = w - buttonWidth;

        juce::FlexBox flexbox;

        flexbox.items.add(juce::FlexItem(buttonWidth, h, *actionButton).withAlignSelf(juce::FlexItem::AlignSelf::center));
        flexbox.items.add(juce::FlexItem(nameWidth, h, *nameValueLabel).withAlignSelf(juce::FlexItem::AlignSelf::center));
        //flexbox.items.add(juce::FlexItem(sizeWidth, h, *sizeValueLabel).withAlignSelf(juce::FlexItem::AlignSelf::center));
        //flexbox.items.add(juce::FlexItem(periodWidth, h, *periodValueLabel).withAlignSelf(juce::FlexItem::AlignSelf::center));

        flexbox.performLayout(getLocalBounds());
    }

    void setDisplayedTuning(const MappedTuningTable* tuning)
    {

        juce::StringArray tokens;

        if (tuning->getName().isNotEmpty())
            tokens.add(tuning->getName());
        else
            tokens.add("untitled");

        if (tuning->getSizeString().isNotEmpty() && tuning->getPeriodString().isNotEmpty())
            tokens.add(tuning->getSizeString() + " steps to " + tuning->getPeriodString());

        else
            tokens.add("Table size: " + juce::String(tuning->getTableSize()));

        mappedTuningSummary = tokens.joinIntoString(" | ");

        nameValueLabel->setText(mappedTuningSummary, juce::NotificationType::dontSendNotification);

        //if (sizeString.isNotEmpty())
        //    sizeString += " steps to ";
        //sizeValueLabel->setText(sizeString, juce::NotificationType::dontSendNotification);

        //periodValueLabel->setText(tuning->getPeriodString(), juce::NotificationType::dontSendNotification);

        resized();
    }

    void setButtonBackState(bool isBackButton)
    {
        buttonIsBack = isBackButton;
        actionButton->setButtonText((buttonIsBack) ? "<" : "=");
        resized();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoBar)
};
