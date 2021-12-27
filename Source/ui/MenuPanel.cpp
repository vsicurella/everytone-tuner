/*
  ==============================================================================

    MenuPanel.cpp
    Created: 27 Dec 2021 1:29:26pm
    Author:  Vincenzo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MenuPanel.h"

//==============================================================================
MenuPanel::MenuPanel(juce::ApplicationCommandManager* cmdManager)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    auto newTuningBtn = menuButtons.add(new juce::TextButton("newTuningButton"));
    newTuningBtn->setCommandToTrigger(cmdManager, Everytone::Commands::NewTuning, true);
    newTuningBtn->setButtonText("New Tuning");
    addAndMakeVisible(*newTuningBtn);

    auto openTuningBtn = menuButtons.add(new juce::TextButton("openTuningBtn"));
    openTuningBtn->setCommandToTrigger(cmdManager, Everytone::Commands::OpenTuning, true);
    openTuningBtn->setButtonText("Open Tuning");
    addAndMakeVisible(*openTuningBtn);

    auto referenceBtn = menuButtons.add(new juce::TextButton("editReferenceBtn"));
    referenceBtn->setCommandToTrigger(cmdManager, Everytone::Commands::EditReference, true);
    referenceBtn->setButtonText("Reference/Mapping");
    addAndMakeVisible(*referenceBtn);

    auto showOptions = menuButtons.add(new juce::TextButton("showOptionsBtn"));
    showOptions->setCommandToTrigger(cmdManager, Everytone::Commands::ShowOptions, true);
    showOptions->setButtonText("Options");
    addAndMakeVisible(*showOptions);
}

MenuPanel::~MenuPanel()
{
    menuButtons.clear();
}

void MenuPanel::paint (juce::Graphics& g)
{

}

void MenuPanel::resized()
{
    auto w = getWidth();
    auto h = getHeight();

    auto numRows = round(menuButtons.size() / 2.0);
    
    auto margin = (w > h) ? w * 0.05f : h * 0.05f;
    
    auto buttonWidth = (w / 2.0f) - (margin * 4);
    auto buttonHeight = (h / numRows) - (margin * 4);

    auto flexMargin = juce::FlexItem::Margin(margin);

    juce::FlexBox flexbox;
    flexbox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexbox.flexDirection = juce::FlexBox::Direction::row;
    flexbox.justifyContent = juce::FlexBox::JustifyContent::center;

    for (auto btn : menuButtons)
    {
        auto item = juce::FlexItem(buttonWidth, buttonHeight, *btn).withMargin(margin);
        flexbox.items.add(item);
    }

    flexbox.performLayout(getLocalBounds());
}
