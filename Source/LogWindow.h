/*
  ==============================================================================

    LogWindow.h
    Created: 20 Nov 2021 5:11:14pm
    Author:  vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LogWindow : public juce::Logger, public juce::DocumentWindow
{
    
    //std::unique_ptr<juce::TextEditor> textBox;
    juce::TextEditor* textBox;

public:

    LogWindow()
        : juce::DocumentWindow("Multimapper Log", 
            juce::Colours::black, 
            juce::DocumentWindow::TitleBarButtons::minimiseButton,
            true)
    {
        textBox = new juce::TextEditor();
        textBox->setMultiLine(true, false);
        textBox->setReadOnly(true);
        textBox->setSize(800, 600);
        setContentOwned(textBox, true);
    }

    ~LogWindow()
    {
        
    }

    void resized() override
    {
        juce::ResizableWindow::resized();
        textBox->setBounds(getLocalBounds());
    }

    void logMessage(const juce::String& msg) override
    {
        textBox->insertTextAtCaret(msg + '\n');
    }

};
