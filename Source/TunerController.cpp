/*
  ==============================================================================

    TunerController.cpp
    Created: 11 Dec 2021 9:43:20pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TunerController.h"

TunerController::TunerController(Everytone::MappingMode modeIn, Everytone::MappingType typeIn)
    : mappingType(typeIn)
{   
    auto standardTuning = std::make_shared<Tuning>(Tuning::StandardTuningDefinition());
    auto mapping = mapForTuning(standardTuning.get(), false);
    setTunings(standardTuning, mapping, standardTuning, mapping);

    mappingMode = modeIn;
}

TunerController::TunerController(CentsDefinition sourceTuning, TuningTableMap::Root sourceMapRootIn,
                                 CentsDefinition targetTuning, TuningTableMap::Root targetMapRootIn, 
                                 Everytone::MappingMode modeIn, Everytone::MappingType typeIn)
    : sourceMapRoot(sourceMapRootIn), 
      targetMapRoot(targetMapRootIn), 
      mappingType(typeIn)
{
    loadSourceTuning(sourceTuning, false);
    loadTargetTuning(targetTuning, true);

    mappingMode = modeIn;
}

TunerController::TunerController(CentsDefinition sourceTuningDefinition, TuningTableMap::Definition sourceMappingDefinition,
                                 CentsDefinition targetTuningDefinition, TuningTableMap::Definition targetMappingDefinition,
                                 Everytone::MappingMode modeIn, Everytone::MappingType typeIn)
    : mappingMode(modeIn), 
      mappingType(typeIn)
{
    auto sourceTuning = std::make_shared<Tuning>(sourceTuningDefinition);
    auto sourceMap = std::make_shared<TuningTableMap>(sourceMappingDefinition);
    
    auto targetTuning = std::make_shared<Tuning>(targetTuningDefinition);
    auto targetMap = std::make_shared<TuningTableMap>(targetMappingDefinition);

    setTunings(sourceTuning, sourceMap, targetTuning, targetMap);
}

TunerController::~TunerController()
{

}

void TunerController::loadSourceTuning(const CentsDefinition& tuningDefinition)
{
    auto newTuning = std::make_shared<Tuning>(tuningDefinition);
    setSourceTuning(newTuning, mapForTuning(newTuning.get(), false));
}

void TunerController::loadTargetTuning(const CentsDefinition& tuningDefinition)
{
    auto newTuning = std::make_shared<Tuning>(tuningDefinition);
    setTargetTuning(newTuning, mapForTuning(newTuning.get(), true));
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

void TunerController::setSourceMapRoot(TuningTableMap::Root root)
{
    sourceMapRoot = root;
    mappingTypeChanged(); // TODO optimize this
}

void TunerController::setTargetMapRoot(TuningTableMap::Root root)
{
    targetMapRoot = root;
    mappingTypeChanged(); // TODO optimize this
}

void TunerController::loadTunings(const CentsDefinition& sourceTuningDefinition, const CentsDefinition& targetTuningDefinition)
{
    auto newSourceTuning = std::make_shared<Tuning>(sourceTuningDefinition);
    auto newTargetTuning = std::make_shared<Tuning>(targetTuningDefinition);

    setTunings(newSourceTuning, mapForTuning(newSourceTuning.get(), false), 
               newTargetTuning, mapForTuning(newTargetTuning.get(), true));
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

void TunerController::loadSourceTuning(const CentsDefinition& definition, bool updateTuner)
{
    auto newTuning = std::make_shared<Tuning>(definition);
    setSourceTuning(newTuning, mapForTuning(newTuning.get(), false), updateTuner);
}

void TunerController::loadTargetTuning(const CentsDefinition& definition, bool updateTuner)
{
    auto newTuning = std::make_shared<Tuning>(definition);
    setTargetTuning(newTuning, mapForTuning(newTuning.get(), true), updateTuner);
}

void TunerController::setSourceTuning(std::shared_ptr<Tuning> tuning, std::shared_ptr<TuningTableMap> mapping, bool updateTuner)
{
    sourceMapRoot = mapping->getRoot();
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
    targetMapRoot = mapping->getRoot();
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
        // TODO only change is something actually changed
        auto sourceTuning = currentTuningSource.shareTuning();
        auto newSourceMapping = mapForTuning(sourceTuning.get(), false);

        auto targetTuning = currentTuningTarget.shareTuning();
        auto newTargetMapping = mapForTuning(targetTuning.get(), true);

        setTunings(sourceTuning, newSourceMapping, targetTuning, newTargetMapping);
    }
}

std::shared_ptr<TuningTableMap> TunerController::mapForTuning(const Tuning* tuning, bool isTarget)
{
    switch (mappingMode)
    {
    case Everytone::MappingMode::Auto:
    {
        auto root = (isTarget) ? targetMapRoot
                               : sourceMapRoot;
        return NewMappingFromTuning(tuning, root, mappingType);
    }
    case Everytone::MappingMode::Manual:
        return (isTarget) ? currentTuningTarget.shareMapping()
                          : currentTuningSource.shareMapping();

    default:
        jassertfalse;
    }
    
    return nullptr;
}

std::shared_ptr<TuningTableMap> TunerController::NewLinearMappingFromTuning(const Tuning* tuning, TuningTableMap::Root root)
{
    auto definition = TuningTableMap::LinearMappingDefinition(root.midiChannel, root.midiNote, tuning->getRootIndex());
    return std::make_shared<TuningTableMap>(definition);
}

std::shared_ptr<TuningTableMap> TunerController::NewPeriodicMappingFromTuning(const Tuning* tuning, TuningTableMap::Root root)
{
    auto definition = MultichannelMap::PeriodicMappingDefinition((int)tuning->getVirtualSize(), tuning->getRootIndex());
    return std::make_shared<TuningTableMap>(definition);
}

std::shared_ptr<TuningTableMap> TunerController::NewMappingFromTuning(const Tuning* tuning, TuningTableMap::Root root, Everytone::MappingType mappingType)
{
    switch (mappingType)
    {
    case Everytone::MappingType::Linear:
        return NewLinearMappingFromTuning(tuning, root);

    case Everytone::MappingType::Periodic:
        return NewPeriodicMappingFromTuning(tuning, root);

    default:
        jassertfalse;
    }

    return nullptr;
};