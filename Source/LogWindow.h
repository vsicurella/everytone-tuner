/*
  ==============================================================================

    LogWindow.h
    Created: 20 Nov 2021 5:11:14pm
    Author:  vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LogWindow : public juce::DocumentWindow, private juce::Timer
{
    juce::TextEditor* textBox;
    std::function<juce::String(void)> callback;

public:

    LogWindow(std::function<juce::String(void)> getLogCallback)
        : callback(getLogCallback),
          juce::DocumentWindow("Multimapper Log", 
            juce::Colours::black, 
            juce::DocumentWindow::TitleBarButtons::minimiseButton,
            true)
    {
        textBox = new juce::TextEditor();
        textBox->setMultiLine(true, false);
        textBox->setReadOnly(true);
        textBox->setSize(800, 600);
        setContentOwned(textBox, true);

        setSize(800, 600);

        startTimer(50);
    }

    ~LogWindow()
    {
        
    }

    void closeButtonPressed() override
    {
        
    }

    void timerCallback() override
    {
        textBox->setText(callback(), false);
    }
};
