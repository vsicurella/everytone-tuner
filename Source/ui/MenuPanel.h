/*
  ==============================================================================

    MenuPanel.h
    Created: 27 Dec 2021 1:29:26pm
    Author:  Vincenzo

    Show buttons to other editing panels

  ==============================================================================
*/

#pragma once

#include "../Common.h"

//==============================================================================
/*
*/
class MenuPanel  : public juce::Component
{
    
public:
    MenuPanel(juce::ApplicationCommandManager* cmdManager);
    ~MenuPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    juce::OwnedArray<juce::TextButton> menuButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuPanel)
};
