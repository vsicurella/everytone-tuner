/*
  ==============================================================================

    MappedTuning.h
    Created: 24 Dec 2021 3:44:29pm
    Author:  Vincenzo

    A wrapper around a Tuning a TuningTableMap

  ==============================================================================
*/

#pragma once
#include "Tuning.h"
#include "../mapping/TuningTableMap.h"

class MappedTuning : public TuningBase
{
    std::shared_ptr<Tuning> tuning;
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
    
    MappedTuning(std::shared_ptr<Tuning> tuning, std::shared_ptr<TuningTableMap> mapping);
    MappedTuning(const MappedTuning& mappedTuning);
    ~MappedTuning();

    // TuningBase implementation

    virtual int getTuningSize() const override;
    
    virtual void setRootFrequency(double frequency) override;
    
    virtual double centsAt(int index) const override;
    virtual double frequencyAt(int index) const override;
    virtual double mtsAt(int index) const override;
  
    virtual int closestIndexToFrequency(double frequency) const override;
    virtual int closestIndexToMts(double mts) const override;

    // MappedTuning declarations

    Tuning* getTuning() const { return tuning.get(); }

    TuningTableMap* getMapping() const { return mapping.get(); }

    std::shared_ptr<Tuning> shareTuning() const { return tuning; }

    std::shared_ptr<TuningTableMap> shareMapping() const { return mapping; }

    MappedTuning::Root getRoot() const
    {
        MappedTuning::Root root =
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

public:

    static std::unique_ptr<MappedTuning> StandardTuning()
    {
        auto standardTuning = std::make_shared<Tuning>(Tuning::StandardTuningDefinition());
        auto standardMapping = std::make_shared<TuningTableMap>(TuningTableMap::StandardMappingDefinition());
        return std::make_unique<MappedTuning>(standardTuning, standardMapping);
    }
};