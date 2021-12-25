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

private:

    int midiIndex(int note, int channelIndex) { return 128 * channelIndex + note; }

public:
    
    MappedTuning();
    MappedTuning(std::shared_ptr<Tuning> tuning, std::shared_ptr<TuningTableMap> mapping);
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



    virtual int tuningIndexAt(int midiNote, int midiChannel);

    virtual double centsAt(int midiNote, int midiChannel);

    virtual double frequencyAt(int midiNote, int midiChannel);

    virtual double mtsAt(int midiNote, int midiChannel);
};