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
    auto standardTuning = std::make_shared<TuningTable>(TuningTable::StandardTuningDefinition());
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
    auto sourceTuning = std::make_shared<TuningTable>(sourceTuningDefinition);
    auto sourceMap = std::make_shared<TuningTableMap>(sourceMappingDefinition);
    
    auto targetTuning = std::make_shared<TuningTable>(targetTuningDefinition);
    auto targetMap = std::make_shared<TuningTableMap>(targetMappingDefinition);

    setTunings(sourceTuning, sourceMap, targetTuning, targetMap);
}

TunerController::~TunerController()
{

}

void TunerController::loadSourceTuning(const CentsDefinition& tuningDefinition)
{
    auto newTuning = std::make_shared<TuningTable>(tuningDefinition);
    setSourceTuning(newTuning, mapForTuning(newTuning.get(), false));
}

void TunerController::loadSourceTuning(const CentsDefinition& tuningDefinition, const TuningTableMap::Definition& mapDefinition)
{
    auto newTuning = std::make_shared<TuningTable>(tuningDefinition);
    auto newMapping = std::make_shared<TuningTableMap>(mapDefinition);
    setSourceTuning(newTuning, newMapping);
}

void TunerController::loadTargetTuning(const CentsDefinition& tuningDefinition)
{
    auto newTuning = std::make_shared<TuningTable>(tuningDefinition);
    setTargetTuning(newTuning, mapForTuning(newTuning.get(), true));
}

void TunerController::loadTargetTuning(const CentsDefinition& tuningDefinition, const TuningTableMap::Definition& mapDefinition)
{
    auto newTuning = std::make_shared<TuningTable>(tuningDefinition);
    auto newMapping = std::make_shared<TuningTableMap>(mapDefinition);
    setTargetTuning(newTuning, newMapping);
}

void TunerController::remapSource(const TuningTableMap::Definition& mapDefinition)
{
    auto newMapping = std::make_shared<TuningTableMap>(mapDefinition);
    setSourceTuning(currentTuningSource->shareTuning(), newMapping);
}

void TunerController::remapTarget(const TuningTableMap::Definition& mapDefinition)
{
    auto newMapping = std::make_shared<TuningTableMap>(mapDefinition);
    setTargetTuning(currentTuningTarget->shareTuning(), newMapping);
}

void TunerController::setSourceRootFrequency(double frequency)
{
    setSourceMappedTuningRoot({ sourceMapRoot.midiChannel, sourceMapRoot.midiNote, frequency });
}

void TunerController::setTargetRootFrequency(double frequency)
{
    setTargetMappedTuningRoot({ targetMapRoot.midiChannel, targetMapRoot.midiNote, frequency });
}

void TunerController::setSourceMapRoot(TuningTableMap::Root root)
{
    setSourceMappedTuningRoot({ root.midiChannel, root.midiNote, currentTuningSource->getRootFrequency() });
}

void TunerController::setTargetMapRoot(TuningTableMap::Root root)
{
    setTargetMappedTuningRoot({ root.midiChannel, root.midiNote, currentTuningTarget->getRootFrequency() });
}

void TunerController::setSourceMappedTuningRoot(MappedTuningTable::Root root)
{
    bool tuningChanged = currentTuningSource->getRootFrequency() != root.frequency;
    bool mappingChanged = sourceMapRoot.midiChannel != root.midiChannel || sourceMapRoot.midiNote != root.midiNote;
    
    auto tuning = currentTuningSource->shareTuning();
    if (tuningChanged)
    {
        auto currentRootIndex = tuning->getRootIndex();
        auto currentDefinition = tuning->getDefinition();
        currentDefinition.rootFrequency = root.frequency;
        tuning = std::make_shared<TuningTable>(currentDefinition);

        mappingChanged = currentRootIndex != tuning->getRootIndex();
    }

    auto mapping = currentTuningSource->shareMapping();
    if (mappingChanged)
    {
        sourceMapRoot = { root.midiChannel, root.midiNote };
        mapping = mapForTuning(tuning.get(), false);
    }

    if (mappingChanged || tuningChanged)
        setTargetTuning(tuning, mapping);
}

void TunerController::setTargetMappedTuningRoot(MappedTuningTable::Root root)
{
    bool tuningChanged = currentTuningTarget->getRootFrequency() != root.frequency;
    bool mappingChanged = targetMapRoot.midiChannel != root.midiChannel || targetMapRoot.midiNote != root.midiNote;

    auto tuning = currentTuningTarget->shareTuning();
    if (tuningChanged)
    {
        auto currentTuningIndex = tuning->getRootIndex();
        auto currentDefinition = tuning->getDefinition();
        currentDefinition.rootFrequency = root.frequency;
        tuning = std::make_shared<TuningTable>(currentDefinition);

        mappingChanged = currentTuningIndex != tuning->getRootIndex();
    }

    auto mapping = currentTuningTarget->shareMapping();
    if (mappingChanged)
    {
        targetMapRoot = { root.midiChannel, root.midiNote };
        mapping = mapForTuning(tuning.get(), true);
    }

    if (mappingChanged || tuningChanged)
        setTargetTuning(tuning, mapping);
}

void TunerController::loadTunings(const CentsDefinition& sourceTuningDefinition, const CentsDefinition& targetTuningDefinition)
{
    auto newSourceTuning = std::make_shared<TuningTable>(sourceTuningDefinition);
    auto newTargetTuning = std::make_shared<TuningTable>(targetTuningDefinition);

    setTunings(newSourceTuning, mapForTuning(newSourceTuning.get(), false), 
               newTargetTuning, mapForTuning(newTargetTuning.get(), true));
}

void TunerController::loadTunings(const CentsDefinition& sourceTuningDefinition, const TuningTableMap::Definition& sourceMapDefinition,
                                  const CentsDefinition& targetTuningDefinition, const TuningTableMap::Definition& targetMapDefinition)
{
    auto newSourceTuning = std::make_shared<TuningTable>(sourceTuningDefinition);
    auto newSourceMapping = std::make_shared<TuningTableMap>(sourceMapDefinition);

    auto newTargetTuning = std::make_shared<TuningTable>(targetTuningDefinition);
    auto newTargetMapping = std::make_shared<TuningTableMap>(targetMapDefinition);

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
    bool typeChanged = mappingType != type;
    if (typeChanged)
    {
        mappingType = type;
        mappingTypeChanged();
    }
}

void TunerController::setPitchbendRange(int pitchbendRangeIn)
{
    if (pitchbendRangeIn > 0 && pitchbendRangeIn < 128)
    {
        pitchbendRange = pitchbendRangeIn;
        juce::Logger::writeToLog("Pitchbend range set to " + juce::String(pitchbendRange));
        updateCurrentTuner();
        return;
    }

    juce::Logger::writeToLog("Pitchbend range of " + juce::String(pitchbendRangeIn) + " was ignored.");
}

void TunerController::loadSourceTuning(const CentsDefinition& definition, bool updateTuner)
{
    auto newTuning = std::make_shared<TuningTable>(definition);
    setSourceTuning(newTuning, mapForTuning(newTuning.get(), false), updateTuner);
}

void TunerController::loadTargetTuning(const CentsDefinition& definition, bool updateTuner)
{
    auto newTuning = std::make_shared<TuningTable>(definition);
    setTargetTuning(newTuning, mapForTuning(newTuning.get(), true), updateTuner);
}

void TunerController::setSourceTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping, bool updateTuner)
{
    sourceMapRoot = mapping->getRoot();
    currentTuningSource = std::make_shared<MappedTuningTable>(tuning, mapping);
    juce::Logger::writeToLog("Loaded new source tuning: " + tuning->getDescription());

    if (updateTuner)
    {
        updateCurrentTuner();
        watchers.call(&Watcher::sourceTuningChanged, currentTuningSource);
    }
}

void TunerController::setTargetTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping, bool updateTuner)
{
    targetMapRoot = mapping->getRoot();
    currentTuningTarget = std::make_shared<MappedTuningTable>(tuning, mapping);
    juce::Logger::writeToLog("Loaded new target tuning: " + tuning->getDescription());

    if (updateTuner)
    {
        updateCurrentTuner();
        watchers.call(&Watcher::targetTuningChanged, currentTuningTarget);
    }
}

void TunerController::setTunings(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTableMap> sourceMapping,
                                 std::shared_ptr<TuningTable> targetTuning, std::shared_ptr<TuningTableMap> targetMapping)
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
        auto sourceTuning = currentTuningSource->shareTuning();
        auto newSourceMapping = mapForTuning(sourceTuning.get(), false);

        auto targetTuning = currentTuningTarget->shareTuning();
        auto newTargetMapping = mapForTuning(targetTuning.get(), true);

        setTunings(sourceTuning, newSourceMapping, targetTuning, newTargetMapping);
    }
}

std::shared_ptr<TuningTableMap> TunerController::mapForTuning(const TuningTable* tuning, bool isTarget)
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
        return (isTarget) ? currentTuningTarget->shareMapping()
                          : currentTuningSource->shareMapping();

    default:
        jassertfalse;
    }
    
    return nullptr;
}

std::shared_ptr<TuningTableMap> TunerController::NewLinearMappingFromTuning(const TuningTable* tuning, TuningTableMap::Root root)
{
    auto definition = TuningTableMap::LinearMappingDefinition(root.midiChannel, root.midiNote, tuning->getRootIndex(), tuning->getTuningTableSize());
    return std::make_shared<TuningTableMap>(definition);
}

std::shared_ptr<TuningTableMap> TunerController::NewPeriodicMappingFromTuning(const TuningTable* tuning, TuningTableMap::Root root)
{
    auto definition = MultichannelMap::PeriodicMappingDefinition((int)tuning->getVirtualSize(), root.midiChannel, root.midiNote, tuning->getRootIndex(), tuning->getTuningTableSize());
    return std::make_shared<TuningTableMap>(definition);
}

std::shared_ptr<TuningTableMap> TunerController::NewMappingFromTuning(const TuningTable* tuning, TuningTableMap::Root root, Everytone::MappingType mappingType)
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