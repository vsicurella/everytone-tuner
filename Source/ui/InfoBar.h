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
    std::unique_ptr<juce::Label> sizeValueLabel;

    bool buttonIsBack = false;

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

        sizeValueLabel = std::make_unique<juce::Label>("sizeLabel", "");
        sizeValueLabel->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*sizeValueLabel);
    }

    ~InfoBar() override
    {
        sizeValueLabel = nullptr;
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
        
        actionButton->setBounds(0, 0, buttonWidth, h);
        sizeValueLabel->setBounds(juce::roundToInt(w - buttonWidth), 0, buttonWidth, h);
        nameValueLabel->setBounds(getLocalBounds().withX(actionButton->getRight()).withRight(sizeValueLabel->getX()));
    }

    void setDisplayedTuning(const MappedTuning* tuning)
    {
        nameValueLabel->setText(tuning->getName(), juce::NotificationType::dontSendNotification);

        // Probably should be abstracted
        auto size = tuning->getTuning()->getVirtualSize();
        if (size == 0)
            size = tuning->getTuningSize();

        sizeValueLabel->setText(juce::String((int)size), juce::NotificationType::dontSendNotification);
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
