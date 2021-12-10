/*
  ==============================================================================

    EqualTemperamentInterface.h
    Created: 9 Dec 2021 8:31:16pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../TuningChanger.h"

//==============================================================================
/*
*/
class EqualTemperamentInterface  : public juce::Component, public TuningChanger, private juce::Slider::Listener
{
public:

    enum PeriodType
    {
        Cents = 0,
        Ratio
    };

private:

    std::unique_ptr<juce::Slider> divisionsSlider;
    std::unique_ptr<juce::Slider> periodSlider;
    
    PeriodType periodType = PeriodType::Cents;

    std::unique_ptr<Tuning> tuning;

public:
    EqualTemperamentInterface()
    {
        divisionsSlider.reset(new juce::Slider(juce::Slider::SliderStyle::IncDecButtons, juce::Slider::TextEntryBoxPosition::TextBoxRight));
        addAndMakeVisible(divisionsSlider.get());
        divisionsSlider->setRange(1.0, 100000.0, 1.0);
        divisionsSlider->setValue(12);

        periodSlider.reset(new juce::Slider(juce::Slider::SliderStyle::IncDecButtons, juce::Slider::TextEntryBoxPosition::TextBoxRight));
        addAndMakeVisible(periodSlider.get());
        periodSlider->setRange(0.0, 120000.0, 100.0);
        periodSlider->setValue(1200.0);
    }

    ~EqualTemperamentInterface() override
    {
        tuning = nullptr;
        periodSlider = nullptr;
        divisionsSlider = nullptr;
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    }

    void resized() override
    {
        int w = getWidth();
        int h = getHeight();

        double eighthHeight = h * 0.125;

        double controlWidth = w * 0.5;

        juce::FlexBox bounds;

        juce::FlexBox controls;
        controls.items.add(juce::FlexItem(controlWidth, eighthHeight, *divisionsSlider));
        controls.items.add(juce::FlexItem(controlWidth, eighthHeight, *periodSlider));

        bounds.items.add(juce::FlexItem(w, h, controls));
        bounds.performLayout(getLocalBounds());
    }

    // can be abstracted
    void recalculateTuning()
    {
        int cents = (periodType == PeriodType::Cents) ? periodSlider->getValue() / divisionsSlider->getValue()
                                                      : ratioToCents(periodSlider->getValue()) / divisionsSlider->getValue();
        auto definition = Tuning::CentsDefinition();
        definition.intervalCents = { cents };
        definition.name = juce::String(cents) + " cET";
        definition.description = juce::String(divisionsSlider->getValue()) + " equal divisions of " + juce::String(periodSlider->getValue()) + " cents";

        tuning.reset(new Tuning(definition));
        tuningWatchers.call(&TuningWatcher::tuningChanged, this, tuning.get());
    }

    void sliderValueChanged(juce::Slider* sliderThatChanged) override
    {
        if (sliderThatChanged == divisionsSlider.get())
        {

        }

        else if (sliderThatChanged == periodSlider.get())
        {

        }

        recalculateTuning();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualTemperamentInterface)
};
