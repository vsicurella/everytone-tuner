/*
  ==============================================================================

    ChannelConfigureComponent.cpp
    Created: 20 Jul 2020 6:09:41pm
    Author:  Vincenzo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ChannelComponent.h"

//==============================================================================
ChannelComponent::ChannelComponent(juce::Array<bool> disabledChannels, juce::String name)
    : juce::Component(name), channelsDisabled(disabledChannels)
{
    channelsDisabled.resize(16);

    // Default colours
    setColour(ChannelComponent::ColourIds::backgroundColourId, juce::Colour());
    setColour(ChannelComponent::ColourIds::channelUnusedColourId, juce::Colours::darkgrey);
    setColour(ChannelComponent::ColourIds::channelUsedColourId, juce::Colours::lightgoldenrodyellow);
    setColour(ChannelComponent::ColourIds::channelOffColourId, juce::Colours::darkgrey);
    setColour(ChannelComponent::ColourIds::channelOnColourId, juce::Colours::cadetblue);
    setColour(ChannelComponent::ColourIds::labelTextColourId, juce::Colours::white);

    // Default layout
    setLayout(layout);
}

ChannelComponent::~ChannelComponent()
{
    removeAllChangeListeners();
}

juce::Array<bool> ChannelComponent::getChannelsDisabled() const
{
    return channelsDisabled;
}

ChannelComponent::ChannelState ChannelComponent::getChannelState(int channelIndex) const
{
    bool manualDisabled = channelsDisabled[channelIndex];

    if (channelMode == Everytone::ChannelMode::Monophonic)
    {
        return (manualDisabled) ? ChannelComponent::ChannelState::ModeDisabled
                                : ChannelComponent::ChannelState::Enabled;
    }
     
    if (   (mpeZone == Everytone::MpeZone::Lower && channelIndex == 0)
        || (mpeZone == Everytone::MpeZone::Upper && channelIndex == 15 ))
            return ChannelComponent::ChannelState::MpeDisabled;

    return (manualDisabled) ? ChannelComponent::ChannelState::ManualDisabled 
                            : ChannelComponent::ChannelState::Enabled;
}

juce::Colour ChannelComponent::getChannelStateColour(ChannelComponent::ChannelState state) const
{
    switch (state)
    {
    case ChannelState::ModeDisabled:
    case ChannelState::MpeDisabled:
        return findColour(ColourIds::channelUnusedColourId);

    case ChannelState::ManualDisabled:
        return findColour(ColourIds::channelOffColourId);

    default:
        return findColour(ColourIds::channelOnColourId);
    }
}

juce::Colour ChannelComponent::getChannelColour(int channelIndex) const
{
    auto channelState = getChannelState(channelIndex);
    return getChannelStateColour(channelState);
}

juce::String ChannelComponent::getChannelStateLabel(ChannelComponent::ChannelState state, int channelIndex) const
{
    switch (state)
    {
    case ChannelState::MpeDisabled:
        return juce::String("G");

    case ChannelState::ModeDisabled:
    case ChannelState::ManualDisabled:
        return juce::String("X");

    default:
        return juce::String(channelIndex + 1);
    }
}

juce::String ChannelComponent::getChannelLabel(int channelIndex) const
{
    auto channelState = getChannelState(channelIndex);
    return getChannelStateLabel(channelState, channelIndex);
}

juce::String ChannelComponent::getChannelStateTooltip(ChannelComponent::ChannelState state, int channelIndex) const
{
    switch (state)
    {
    case ChannelComponent::ChannelState::MpeDisabled:
        return juce::String("Global MPE channel");

    case ChannelComponent::ChannelState::ModeDisabled:
    {
        auto monoChannel = channelsDisabled.indexOf(false);
        return juce::String("Monophonic mode on channel ") + juce::String(monoChannel + 1);
    }

    case ChannelComponent::ChannelState::ManualDisabled:
        return juce::String("Channel ") + juce::String(channelIndex + 1) + juce::String(" Disabled");

    default:
        return juce::String();
    }
}

void ChannelComponent::channelsChangedCallback()
{
    sendChangeMessage();
    onChange();
}

void ChannelComponent::paint (juce::Graphics& g)
{
    g.fillAll(findColour(ChannelComponent::ColourIds::backgroundColourId));

    auto labelFont = juce::Font().withStyle(juce::Font::FontStyleFlags::bold);

    juce::Rectangle<float> chRect;
    for (int ch = 0; ch < 16; ch++)
    {
        auto state = getChannelState(ch);
        juce::Colour channelColour = getChannelStateColour(state);

        chRect = channelRectangles[ch];
        
        if (ch == mouseOverChannel)
        {
            switch (state)
            {
            case ChannelComponent::ChannelState::Enabled:
            case ChannelComponent::ChannelState::ManualDisabled:
            case ChannelComponent::ChannelState::ModeDisabled:
                channelColour = channelColour.contrasting(0.25f);
                break;

            default:
                break;
            }
        }

        // TODO: check if channel has note (or do callback)

        g.setColour(channelColour);
        g.fillRect(chRect);

        g.setColour(juce::Colours::black);
        g.drawRect(chRect);

        g.setColour(channelColour.contrasting());
        g.setFont(labelFont);

        auto label = getChannelLabel(ch);
        g.drawFittedText(label, chRect.toNearestInt(), juce::Justification::centred,1, 0.01f);
    }

    g.setColour(juce::Colours::black);
    g.drawRect(0.0f, 0.0f, (float)getWidth(), (float)getHeight(), 1.0f);
}

void ChannelComponent::resized()
{
    auto w = getWidth();
    auto h = getHeight();

    int squareWidth = juce::roundToInt((w - labelWidth * 2) / (float)sizeFactor);

    // adjust size from rounding error
    labelWidth = (w - squareWidth * sizeFactor) * 0.5;
    channelControlWidth = w - labelWidth * 2;
    squareWidth = channelControlWidth / sizeFactor;

    channelSize.setXY(squareWidth, round(h / otherSizeFactor));

    updateChannelRectangles();

    //auto flexbox = juce::FlexBox(juce::FlexBox::Direction::row,
    //                             juce::FlexBox::Wrap::wrap,
    //                             juce::FlexBox::AlignContent::flexStart,
    //                             juce::FlexBox::AlignItems::flexStart,
    //                             juce::FlexBox::JustifyContent::flexStart);

}

void ChannelComponent::mouseMove(const juce::MouseEvent& event)
{
    int channelNum = findChannelMouseOver(event);
    
    if (mouseOverChannel != channelNum)
    {
        mouseOverChannel = channelNum;
        repaint();
    }
}

void ChannelComponent::mouseDown(const juce::MouseEvent& event)
{
    lastChannelMouseClicked = mouseOverChannel;

    auto channelState = getChannelState(lastChannelMouseClicked);
    switch (channelState)
    {
    case ChannelComponent::ChannelState::Enabled:
        channelsDisabled.set(lastChannelMouseClicked, true);
        break;

    case ChannelComponent::ChannelState::ManualDisabled:
        channelsDisabled.set(lastChannelMouseClicked, false);
        break;

    case ChannelComponent::ChannelState::ModeDisabled:
        setMonophonicChannelIndex(lastChannelMouseClicked);
        break;

    default:
        break;
    }
    
    channelsChangedCallback();
    repaint();
}

void ChannelComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (!getLocalBounds().contains(event.getPosition()))
        return;

    int channelNum = findChannelMouseOver(event);

    if (mouseOverChannel != channelNum)
    {
        mouseOverChannel = channelNum;
        mouseDown(event);
    }
}

void ChannelComponent::mouseExit(const juce::MouseEvent& event)
{
    mouseOverChannel = -1;
    repaint();
}

juce::String ChannelComponent::getTooltip()
{
    if (mouseOverChannel >= 0)
    {
        auto state = getChannelState(mouseOverChannel);
        return getChannelStateTooltip(state, mouseOverChannel);
    }

    return juce::String();
}

void ChannelComponent::channelModeChanged(Everytone::ChannelMode newChannelMode)
{
    setChannelMode(newChannelMode);
}

void ChannelComponent::mpeZoneChanged(Everytone::MpeZone zone)
{
    setMpeZone(zone);
}

void ChannelComponent::setChannelMode(Everytone::ChannelMode newChannelMode)
{
    if (newChannelMode == Everytone::ChannelMode::Monophonic)
    {
        int firstEnabledChannel = channelsDisabled.indexOf(false);
        setMonophonicChannelIndex(firstEnabledChannel);
    }

    else if (channelMode == Everytone::ChannelMode::Monophonic)
    {
        channelsDisabled.fill(false);
        channelsChangedCallback();
    }

    channelMode = newChannelMode;
    repaint();

}
void ChannelComponent::setMpeZone(Everytone::MpeZone zone)
{
    mpeZone = zone;
    repaint();
}

void ChannelComponent::setChannelsDisabled(juce::Array<bool> channelsDisabledIn)
{
    channelsDisabled = channelsDisabledIn;
    sendChangeMessage();
    resized();
}

void ChannelComponent::setLabelWidth(int widthIn)
{
    labelWidth = widthIn;

    if (!getBounds().isEmpty())
        resized();
}

void ChannelComponent::setLayout(ChannelComponent::Layout layoutIn)
{
    layout = layoutIn;

    sizeFactor = 16 / pow(2, (int)layout);
    otherSizeFactor = 16 / sizeFactor;

    //switch (layout)
    //{
    //case ChannelComponent::Layout::Line:
    //    channelsLabelled = { 0, 15 };
    //    break;

    //case ChannelComponent::Layout::Square:
    //    channelsLabelled = { 0, 3, 4, 7, 8, 11, 14, 15 };
    //    break;

    //default: // ChannelConfigureComponent::Layout::Rectangle
    //    channelsLabelled = { 0, 7, 8, 15 };
    //    bre
    //}

    updateLayoutFormat();
}

void ChannelComponent::setOrientation(ChannelComponent::Orientation orientationIn)
{
    orientation = orientationIn;
    updateLayoutFormat();
}

int ChannelComponent::findChannelMouseOver(const juce::MouseEvent& eventIn) const
{
    int channelNum = -1;

    if (channelBounds.contains(eventIn.getPosition()))
    {
        channelNum = (eventIn.x - labelWidth) / channelSize.x + eventIn.y / channelSize.y * 8;
    }

    return channelNum;
}

void ChannelComponent::updateChannelRectangles()
{
    channelRectangles.clear();
    int channelNum = 0;
    for (int row = 0; row < format.y; row++)
    {
        for (int col = 0; col < format.x; col++)
        {
            juce::Point<float> cornerTL = juce::Point<float>(labelWidth + col * channelSize.x, row * channelSize.y);
            juce::Point<float> cornerBR = juce::Point<float>(cornerTL.x + channelSize.x, cornerTL.y + channelSize.y);
            channelRectangles.add(juce::Rectangle<float>(cornerTL, cornerBR));
        }
    }

    channelBounds = juce::Rectangle<int>(labelWidth, 0, channelControlWidth, getHeight());
}

void ChannelComponent::setMonophonicChannelIndex(int channelIndex)
{
    for (int ch = 0; ch < 16; ch++)
        channelsDisabled.set(ch, ch != channelIndex);

    channelsChangedCallback();
}

void ChannelComponent::updateLayoutFormat()
{
    format = (orientation == ChannelComponent::Orientation::Horizontal) ? juce::Point<int>(sizeFactor, otherSizeFactor) // Horizontal
                                                                        : juce::Point<int>(otherSizeFactor, sizeFactor); // Vertical
    resized();
}
