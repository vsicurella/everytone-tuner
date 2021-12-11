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
class EqualTemperamentInterface  : public juce::Component, 
                                   public TuningChanger
                                   //private juce::Slider::Listener
{
public:

    enum PeriodType
    {
        Cents = 1,
        Ratio
    };

private:

    std::unique_ptr<juce::Slider> divisionsSlider;
    std::unique_ptr<juce::Label> divisionsLabel;

    std::unique_ptr<juce::Slider> periodSlider;
    std::unique_ptr<juce::Label> periodLabel;

    std::unique_ptr<juce::ComboBox> periodTypeBox;
    
    PeriodType periodType = PeriodType::Cents;

    std::unique_ptr<Tuning> tuning;

public:

    void setPeriodType(PeriodType typeIn)
    {
        auto period = periodSlider->getValue();

        switch (periodType)
        {
        case PeriodType::Cents:
            if (typeIn == PeriodType::Ratio)
            {
                auto newPeriod = centsToRatio(period);
                periodSlider->setValue(newPeriod, juce::NotificationType::dontSendNotification);
            }
            break;

        case PeriodType::Ratio:
            if (typeIn == PeriodType::Cents)
            {
                auto newPeriod = ratioToCents(period);
                periodSlider->setValue(newPeriod, juce::NotificationType::dontSendNotification);
            }
            break;


        default:

            jassertfalse;
        }

        periodType = typeIn;
    }

    // can be abstracted
    void recalculateTuning()
    {
        juce::String description = juce::String(divisionsSlider->getValue()) + " equal divisions of ";

        double periodCents = periodSlider->getValue();
        description += juce::String(periodCents);

        if (periodType == PeriodType::Ratio)
        {
            periodCents = ratioToCents(periodCents);
        }
        else
        {
            description += " cents.";
        }

        double cents = periodCents / divisionsSlider->getValue();

        auto definition = Tuning::CentsDefinition();
        definition.intervalCents = { cents };
        definition.name = juce::String(roundN(3, cents)) + " cET";
        definition.description = description;

        tuning.reset(new Tuning(definition));
        tuningWatchers.call(&TuningWatcher::tuningChanged, this, tuning.get());
    }

public:
    EqualTemperamentInterface()
    {
        divisionsSlider.reset(new juce::Slider(juce::Slider::SliderStyle::IncDecButtons, juce::Slider::TextEntryBoxPosition::TextBoxLeft));
        addAndMakeVisible(divisionsSlider.get());
        divisionsSlider->setRange(1.0, 100000.0, 1e-3);
        divisionsSlider->setValue(12);
        divisionsSlider->onValueChange = [&]() { this->recalculateTuning(); };

        divisionsLabel.reset(new juce::Label("DivisionsLabel", juce::translate("Divisions") + ":"));
        divisionsLabel->attachToComponent(divisionsSlider.get(), false);
        addAndMakeVisible(*divisionsLabel);

        periodSlider.reset(new juce::Slider(juce::Slider::SliderStyle::IncDecButtons, juce::Slider::TextEntryBoxPosition::TextBoxLeft));
        addAndMakeVisible(periodSlider.get());
        periodSlider->setRange(0.0, 120000.0, 1e-3);
        periodSlider->setValue(1200.0);
        periodSlider->onValueChange = [&]() { this->recalculateTuning(); };

        periodLabel.reset(new juce::Label("PeriodLabel", juce::translate("Period") + ":"));
        periodLabel->attachToComponent(periodSlider.get(), false);
        addAndMakeVisible(*periodLabel);

        periodTypeBox.reset(new juce::ComboBox("PeriodTypeBox"));
        periodTypeBox->addItem("cents", PeriodType::Cents);
        periodTypeBox->addItem("ratio", PeriodType::Ratio);
        periodTypeBox->setSelectedId(PeriodType::Cents, juce::NotificationType::dontSendNotification);
        periodTypeBox->onChange = [&]() { this->setPeriodType(PeriodType(periodTypeBox->getSelectedId())); };
        addAndMakeVisible(*periodTypeBox);
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
        controls.flexDirection = juce::FlexBox::Direction::column;

        auto controlMargin = juce::FlexItem::Margin(h * 0.15, 0, 0, 0);

        divisionsLabel->setSize(controlWidth, eighthHeight);
        controls.items.add(juce::FlexItem(controlWidth, eighthHeight, *divisionsSlider).withMargin(controlMargin));

        periodLabel->setSize(controlWidth, eighthHeight);
        controls.items.add(juce::FlexItem(controlWidth, eighthHeight, *periodSlider).withMargin(controlMargin));

        bounds.items.add(juce::FlexItem(controlWidth, h, controls).withMargin(5));
        bounds.performLayout(getLocalBounds());

        periodTypeBox->setBounds(periodSlider->getBounds().translated(periodSlider->getWidth(), 0).withWidth(w * 0.2));

        divisionsSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxLeft, false, controlWidth * 0.5, divisionsSlider->getHeight());
        periodSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxLeft, false, controlWidth * 0.5, periodSlider->getHeight());
        
    }

    //void sliderValueChanged(juce::Slider* sliderThatChanged) override
    //{
    //    if (sliderThatChanged == divisionsSlider.get())
    //    {

    //    }

    //    else if (sliderThatChanged == periodSlider.get())
    //    {

    //    }

    //    recalculateTuning();
    //}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualTemperamentInterface)
};
