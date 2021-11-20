/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultimapperAudioProcessorEditor::MultimapperAudioProcessorEditor (MultimapperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    size.reset(new juce::Slider(juce::Slider::SliderStyle::IncDecButtons,juce::Slider::TextEntryBoxPosition::TextBoxLeft));
    size->setRange(1, 1200, 1);
    size->setValue(12);
    addAndMakeVisible(size.get());
    components.add(size.get());

    period.reset(new juce::Slider(juce::Slider::SliderStyle::LinearHorizontal,juce::Slider::TextEntryBoxPosition::TextBoxLeft));
    period->setRange(1, 144000, 0.001);
    period->setValue(1200.0);
    addAndMakeVisible(period.get());
    components.add(period.get());

    rootNote.reset(new juce::Slider(juce::Slider::SliderStyle::IncDecButtons,juce::Slider::TextEntryBoxPosition::TextBoxLeft));
    rootNote->setRange(0, 128, 1);
    rootNote->setValue(60);
    addAndMakeVisible(rootNote.get());
    components.add(rootNote.get());

    rootChannel.reset(new juce::Slider(juce::Slider::SliderStyle::IncDecButtons,juce::Slider::TextEntryBoxPosition::TextBoxLeft));
    rootChannel->setRange(1, 16, 1);
    rootChannel->setValue(1);
    addAndMakeVisible(rootChannel.get());
    components.add(rootChannel.get());

    rootFrequency.reset(new juce::Slider(juce::Slider::SliderStyle::LinearHorizontal,juce::Slider::TextEntryBoxPosition::TextBoxLeft));
    rootFrequency->setRange(20, 12000, 0.001);
    rootFrequency->setValue(261.3);
    addAndMakeVisible(rootFrequency.get());
    components.add(rootFrequency.get());

    setSize (1024, 768);

    logger.reset(new LogWindow());
    logger->setSize(800, 600);
    logger->setVisible(true);
}

MultimapperAudioProcessorEditor::~MultimapperAudioProcessorEditor()
{
    size            = nullptr;
    period          = nullptr;
    rootNote        = nullptr;
    rootChannel     = nullptr;
    rootFrequency   = nullptr;
    logger          = nullptr;
}

//==============================================================================
void MultimapperAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MultimapperAudioProcessorEditor::resized()
{
    int w = getWidth();
    int h = getHeight();

    int componentWidth = juce::roundToInt(w * 0.67f);

    juce::FlexBox flex;
    flex.flexDirection = juce::FlexBox::Direction::column;

    int x = w * 0.1f;
    int y = 60;
    int i = 0;
    for (auto* component : components)
    {
        // component->setBounds(x, y + i * 60, componentWidth, 60);
        auto item = juce::FlexItem(*component);
        flex.items.add(item.withWidth(componentWidth)
                           .withMaxHeight(60)
                           .withFlex(1.0f));
        // i++;
    }

    flex.performLayout(getLocalBounds().toFloat());
}
