/*
  ==============================================================================

    ChannelConfigureComponent.h
    Created: 20 Jul 2020 6:09:41pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Common.h"

//==============================================================================
/*
    A grid representing MIDI channels with mouse interaction to change channel properties.
*/
class ChannelComponent : public juce::Component,
                         public juce::ChangeBroadcaster,
    public juce::TooltipClient,
                         public OptionsWatcher
{
public:

    enum ColourIds
    {
        backgroundColourId      = 0x0000100,
        channelUnusedColourId   = 0x0000200,
        channelUsedColourId     = 0x0000201,
        channelOffColourId      = 0x0000210,
        channelOnColourId       = 0x0000211,
        labelTextColourId       = 0x0000300
    };

    enum class Layout
    {
        Line = 0,
        Rectangle,
        Square
    };

    enum class Orientation
    {
        Horizontal = 0,
        Vertical
    };

    enum class ChannelState
    {
        Enabled = 0,
        ManualDisabled, // Disabled by user
        MpeDisabled,    // Disabled by MpeZone
        ModeDisabled    // Disabled by ChannelMode
    };

private:

    void updateChannelRectangles();

    ChannelState getChannelState(int channelIndex) const;

    juce::Colour getChannelStateColour(ChannelComponent::ChannelState state) const;
    juce::Colour getChannelColour(int channelIndex) const;

    juce::String getChannelStateLabel(ChannelComponent::ChannelState state, int channelIndex) const;
    juce::String getChannelLabel(int channelIndex) const;

    juce::String getChannelStateTooltip(ChannelComponent::ChannelState state, int channelIndex) const;

    int findChannelMouseOver(const juce::MouseEvent& eventIn) const;

    void updateLayoutFormat();

    void setMonophonicChannelIndex(int channelIndex);

    void channelsChangedCallback();

public:

    std::function<void(void)> onChange = [=]() {};

public:

    ChannelComponent(juce::Array<bool> disabledChannels = juce::Array<bool>(), juce::String name = juce::String());
    ~ChannelComponent() override;

    juce::Array<bool> getChannelsDisabled() const;

    void paint (juce::Graphics&) override;
    void resized() override;

    void mouseMove(const juce::MouseEvent& event) override;

    void mouseDown(const juce::MouseEvent& event) override;

    void mouseDrag(const juce::MouseEvent& event) override;

    void mouseExit(const juce::MouseEvent& event) override;

    juce::String getTooltip() override;

    //====================================================================================
    // OptionsWatcher implementation

    void channelModeChanged(Everytone::ChannelMode newChannelMode) override;
    void mpeZoneChanged(Everytone::MpeZone zone) override;

    //====================================================================================

    void setChannelMode(Everytone::ChannelMode newChannelMode);

    void setMpeZone(Everytone::MpeZone zone);

    void setChannelsDisabled(juce::Array<bool> channelsDisabled);

    void setLabelWidth(int widthIn);

    void setLayout(ChannelComponent::Layout layoutIn);

    void setOrientation(ChannelComponent::Orientation orientationIn);

private:

    ChannelComponent::Layout layout = ChannelComponent::Layout::Rectangle;
    ChannelComponent::Orientation orientation = ChannelComponent::Orientation::Horizontal;

    Everytone::ChannelMode channelMode;
    Everytone::MpeZone mpeZone;

    juce::Array<bool> channelsDisabled;

    int labelWidth = 0;
    juce::Point<int> channelSize;

    int sizeFactor;
    int otherSizeFactor;
    juce::Point<int> format;

    juce::Array<juce::Rectangle<float>> channelRectangles;
    juce::Array<bool> channelsLabelled;

    int lastChannelMouseClicked = -1;
    int mouseOverChannel = -1;
    juce::Point<double> mousePosition;

    // DRAWING HELPERS
    int channelControlWidth;
    juce::Rectangle<int> channelBounds;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelComponent)
};
