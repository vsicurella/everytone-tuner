/*
  ==============================================================================

    MappedTuning.h
    Created: 24 Dec 2021 3:44:29pm
    Author:  Vincenzo

    A wrapper around a Tuning a TuningTableMap

  ==============================================================================
*/

#pragma once
#include "FunctionalTuning.h"
#include "../mapping/TuningTableMap.h"

class MappedTuningTable : public TuningTableBase
{
    std::shared_ptr<TuningTable> tuning;
    std::shared_ptr<TuningTableMap> mapping;

public:

    struct Root
    {
        int midiChannel = 1;
        int midiNote = 69;
        double frequency = 440.0;
    };

private:

    int midiIndex(int note, int channelIndex) { return 128 * channelIndex + note; }

public:
    
    MappedTuningTable(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping);
    MappedTuningTable(const MappedTuningTable& mappedTuning);
    ~MappedTuningTable();

    // MappedTuning declarations

    TuningTableBase* getTuning() const { return tuning.get(); }

    TuningTableMap* getMapping() const { return mapping.get(); }

    std::shared_ptr<TuningTable> shareTuning() const { return tuning; }

    std::shared_ptr<TuningTableMap> shareMapping() const { return mapping; }

    MappedTuningTable::Root getRoot() const
    {
        MappedTuningTable::Root root =
        {
            mapping->getRootMidiChannel(),
            mapping->getRootMidiNote(),
            tuning->getRootFrequency()
        };
        return root;
    }


    virtual int tuningIndexAt(int midiNote, int midiChannel) const;

    virtual double centsAt(int midiNote, int midiChannel) const;

    virtual double frequencyAt(int midiNote, int midiChannel) const;

    virtual double mtsAt(int midiNote, int midiChannel) const;


    // TuningTableBase implementation

    virtual int getTableSize() const override;

    virtual double getVirtualPeriod() const override;
    virtual double getVirtualSize() const override;

    virtual double getRootMts() const override;

    virtual juce::Array<double> getFrequencyTable() const override;
    virtual juce::Array<double> getMtsTable() const override;

    // TuningBase implementation

    virtual int getTuningSize() const override;

    virtual void setRootFrequency(double frequency) override;
    std::shared_ptr<TuningTable> setRootFrequency(double frequency, bool returnNewTuning);

    virtual double centsAt(int index) const override;
    virtual double frequencyAt(int index) const override;
    virtual double mtsAt(int index) const override;

    virtual int closestIndexToFrequency(double frequency) const override;
    virtual int closestIndexToMts(double mts) const override;


public:

    static std::unique_ptr<MappedTuningTable> StandardTuning()
    {
        auto standardTuning = std::make_shared<TuningTable>(TuningTable::StandardTuningDefinition());
        auto standardMapping = std::make_shared<TuningTableMap>(TuningTableMap::StandardMappingDefinition());
        return std::make_unique<MappedTuningTable>(standardTuning, standardMapping);
    }
};