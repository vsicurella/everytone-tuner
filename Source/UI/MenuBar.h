/*
  ==============================================================================

    MenuBar.h
    Created: 5 Dec 2021 9:24:22pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MenuBarModel : public juce::MenuBarModel
{
protected:

    enum MenuNames
    {
        File,
        Options
    };


public:

    MenuBarModel(juce::ApplicationCommandManager* cmdManagerIn = nullptr) : cmdManager(cmdManagerIn) {}
    ~MenuBarModel() override {}

    juce::StringArray MenuBarModel::getMenuBarNames() override
    {
        return menuNames;
    }

    juce::PopupMenu MenuBarModel::getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) override
    {
        juce::PopupMenu menu;

        switch (topLevelMenuIndex)
        {
        case MenuNames::File:
            menu.addCommandItem(cmdManager, Multimapper::Commands::NewTuning, "New");
            menu.addCommandItem(cmdManager, Multimapper::Commands::LoadTuning, "Load");
            break;
        case MenuNames::Options:
            menu.addItem("Midi", [&]() {});
            break;

        default:
            break;
        }

        return menu;
    }

    void MenuBarModel::menuItemSelected(int menuItemID, int topLevelMenuIndex) override
    {

    }

private:

    juce::ApplicationCommandManager* cmdManager;

    juce::StringArray menuNames = { "File", "Options" };

    juce::PopupMenu::Options fileMenu;
    juce::PopupMenu::Options optionsMenu;
};