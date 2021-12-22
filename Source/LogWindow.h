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
    juce::StringArray msgsToLog;

public:

    LogWindow(juce::StringArray previousLogs=juce::StringArray())
        : juce::DocumentWindow("Everytone Log", 
            juce::Colours::black, 
            juce::DocumentWindow::TitleBarButtons::minimiseButton,
            true)
    {
        auto textBox = new juce::TextEditor();
        textBox->setMultiLine(true, false);
        textBox->setReadOnly(true);
        textBox->setSize(800, 600);
        setContentOwned(textBox, true);

        for (auto msg : previousLogs)
            textBox->insertTextAtCaret(msg + juce::newLine);

        setSize(800, 600);

        startTimer(100);
    }

    ~LogWindow()
    {
        
    }

    void closeButtonPressed() override
    {
        
    }

    juce::TextEditor* getEditor() const
    {
        return dynamic_cast<juce::TextEditor*>(getContentComponent());
    }

    void addMessage(juce::String string)
    {
        msgsToLog.add(string);
    }

    void timerCallback() override
    {
        auto editor = getEditor();

        for (auto msg : msgsToLog)
            editor->insertTextAtCaret(msg + juce::newLine);

        msgsToLog.clear();
        // i think there is still a race condition issue here
    }
};
