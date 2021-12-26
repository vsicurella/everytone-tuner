/*
  ==============================================================================

    TunerController.cpp
    Created: 11 Dec 2021 9:43:20pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TunerController.h"

TunerController::TunerController()
{
    auto standardTuning = std::make_shared<Tuning>(Tuning::StandardTuning());
    auto standardMapping = std::make_shared<TuningTableMap>(TuningTableMap::StandardMapping());
    setTunings(standardTuning, standardMapping, standardTuning, standardMapping);
}

TunerController::TunerController(CentsDefinition targetTuning, TuningTableMap* targetMapping)
{
    auto standardTuning = std::make_shared<Tuning>(Tuning::StandardTuning());
    auto standardMapping = std::make_shared<TuningTableMap>(TuningTableMap::StandardMapping());
    setSourceTuning(standardTuning, standardMapping, false);
    loadTargetTuning(targetTuning);
}

TunerController::~TunerController()
{

}

void TunerController::loadSourceTuning(const CentsDefinition& tuningDefinition)
{
    auto newTuning = std::make_shared<Tuning>(tuningDefinition);

    switch (mappingMode)
    {
        case Everytone::MappingMode::Auto:
        {    
            auto newMapping = std::make_shared<TuningTableMap>(*newTuningMap(newTuning.get()));
            setSourceTuning(newTuning, newMapping);
            break;
        }
        case Everytone::MappingMode::Manual:
        {
            setSourceTuning(newTuning, currentTuningSource.shareMapping());
            break;
        }
        
        default:
            jassertfalse;
    }
}

void TunerController::loadTargetTuning(const CentsDefinition& tuningDefinition)
{
    auto newTuning = std::make_shared<Tuning>(tuningDefinition);

    switch (mappingMode)
    {
        case Everytone::MappingMode::Auto:
        {
            auto newMapping = std::make_shared<TuningTableMap>(*newTuningMap(newTuning.get()));
            setTargetTuning(newTuning, newMapping);
            break;
        }
        case Everytone::MappingMode::Manual:
        {
            setTargetTuning(newTuning, currentTuningTarget.shareMapping());
            break;
        }
        default:
            jassertfalse;
    }
}

void TunerController::remapSource(const TuningTableMap::Definition& mapDefinition)
{
    auto newMapping = std::make_shared<TuningTableMap>(mapDefinition);
    setSourceTuning(currentTuningSource.shareTuning(), newMapping);
}

void TunerController::remapTarget(const TuningTableMap::Definition& mapDefinition)
{
    auto newMapping = std::make_shared<TuningTableMap>(mapDefinition);
    setTargetTuning(currentTuningTarget.shareTuning(), newMapping);
}

void TunerController::setSourceRootFrequency(double frequency)
{
    auto currentDefinition = currentTuningSource.getTuning()->getDefinition();
    currentDefinition.rootFrequency = frequency;

    auto newTuning = std::make_shared<Tuning>(currentDefinition);
    setSourceTuning(newTuning, currentTuningSource.shareMapping());
}

void TunerController::setTargetRootFrequency(double frequency)
{
    auto currentDefinition = currentTuningTarget.getTuning()->getDefinition();
    currentDefinition.rootFrequency = frequency;

    auto newTuning = std::make_shared<Tuning>(currentDefinition);
    setTargetTuning(newTuning, currentTuningTarget.shareMapping());
}

void TunerController::setTunings(const CentsDefinition& sourceTuningDefinition, const CentsDefinition& targetTuningDefinition)
{
    auto newSourceTuning = std::make_shared<Tuning>(sourceTuningDefinition);
    auto newTargetTuning = std::make_shared<Tuning>(targetTuningDefinition);

    std::shared_ptr<TuningTableMap> newSourceMapping;
    std::shared_ptr<TuningTableMap> newTargetMapping;

    switch (mappingMode)
    {
        case Everytone::MappingMode::Auto:
            newSourceMapping = std::make_shared<TuningTableMap>(*newTuningMap(newSourceTuning.get()));
            newTargetMapping = std::make_shared<TuningTableMap>(*newTuningMap(newTargetTuning.get()));
            break;

        case Everytone::MappingMode::Manual:
            newSourceMapping = currentTuningSource.shareMapping();
            newTargetMapping = currentTuningTarget.shareMapping();
            break;

        default:
            jassertfalse;
    }

    setTunings(newSourceTuning, newSourceMapping, newTargetTuning, newTargetMapping);
}

void TunerController::updateCurrentTuner()
{
    currentTuner = std::make_shared<MidiNoteTuner>(currentTuningSource, currentTuningTarget, pitchbendRange);
}

void TunerController::setMappingMode(Everytone::MappingMode mode)
{
    mappingMode = mode;
}

void TunerController::setMappingType(Everytone::MappingType type)
{
    mappingType = type;
    mappingTypeChanged();
}

void TunerController::setPitchbendRange(int pitchbendRangeIn)
{
    if (pitchbendRangeIn > 0 && pitchbendRangeIn < 128)
    {
        pitchbendRange = pitchbendRangeIn;
        updateCurrentTuner();
    }
}

void TunerController::setSourceTuning(std::shared_ptr<Tuning> tuning, std::shared_ptr<TuningTableMap> mapping, bool updateTuner)
{
    currentTuningSource = MappedTuning(tuning, mapping);
    juce::Logger::writeToLog("Loaded new source tuning: " + tuning->getDescription());

    if (updateTuner)
    {
        updateCurrentTuner();
        watchers.call(&Watcher::sourceTuningChanged, currentTuningSource);
    }
}

void TunerController::setTargetTuning(std::shared_ptr<Tuning> tuning, std::shared_ptr<TuningTableMap> mapping, bool updateTuner)
{
    currentTuningTarget = MappedTuning(tuning, mapping);
    juce::Logger::writeToLog("Loaded new target tuning: " + tuning->getDescription());

    if (updateTuner)
    {
        updateCurrentTuner();
        watchers.call(&Watcher::targetTuningChanged, currentTuningTarget);
    }
}


void TunerController::setTunings(std::shared_ptr<Tuning> sourceTuning, std::shared_ptr<TuningTableMap> sourceMapping,
                                        std::shared_ptr<Tuning> targetTuning, std::shared_ptr<TuningTableMap> targetMapping)
{
    setSourceTuning(sourceTuning, sourceMapping, false);
    setTargetTuning(targetTuning, targetMapping, false);
    updateCurrentTuner();

    // combine into one call?
    watchers.call(&Watcher::sourceTuningChanged, currentTuningSource);
    watchers.call(&Watcher::targetTuningChanged, currentTuningTarget);
}

void TunerController::mappingTypeChanged()
{
    if (mappingMode == Everytone::MappingMode::Auto)
    {
        auto sourceTuning = currentTuningSource.shareTuning();
        auto newSourceMapping = std::make_shared<TuningTableMap>(*newTuningMap(sourceTuning.get()));

        auto targetTuning = currentTuningTarget.shareTuning();
        auto newTargetMapping = std::make_shared<TuningTableMap>(*newTuningMap(targetTuning.get()));

        setTunings(sourceTuning, newSourceMapping, targetTuning, newTargetMapping);
    }
}

std::unique_ptr<TuningTableMap> TunerController::newTuningMap(const Tuning* tuning)
{
    return newTuningMap(tuning, mappingType);
}

std::unique_ptr<TuningTableMap> TunerController::NewLinearMappingFromTuning(const Tuning* tuning)
{
    auto linearMap = std::make_unique<TuningTableMap>(TuningTableMap::CreateLinearMidiMapping(tuning->getRootIndex()));
    return linearMap;
}

std::unique_ptr<TuningTableMap> TunerController::NewPeriodicMappingFromTuning(const Tuning* tuning)
{
    auto periodicMap = std::make_unique<TuningTableMap>(MultichannelMap::CreatePeriodicMapping((int)tuning->getVirtualSize(), tuning->getRootIndex()));
    return periodicMap;
}

std::unique_ptr<TuningTableMap> TunerController::newTuningMap(const Tuning* tuning, Everytone::MappingType mappingType)
{
    switch (mappingType)
    {
    case Everytone::MappingType::Linear:
    {
        return NewLinearMappingFromTuning(tuning);
    }

    case Everytone::MappingType::Periodic:
    {
        return NewPeriodicMappingFromTuning(tuning);
    }

    default:
        jassertfalse;
    }

    return nullptr;
}