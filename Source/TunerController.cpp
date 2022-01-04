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
    auto standardTuning = FunctionalTuning::StandardTuning();
    auto mapping = mapForTuning(standardTuning.get(), false);
    setTunings(standardTuning, mapping, standardTuning, mapping);

    mappingMode = modeIn;
}

TunerController::TunerController(std::shared_ptr<TuningTable> sourceTuning, TuningTableMap::Root sourceMapRootIn,
                                 std::shared_ptr<TuningTable> targetTuning, TuningTableMap::Root targetMapRootIn,
                                 Everytone::MappingMode modeIn, Everytone::MappingType typeIn)
    : sourceMapRoot(sourceMapRootIn), 
      targetMapRoot(targetMapRootIn), 
      mappingType(typeIn)
{
    // Keep default MappingMode::Auto here
    setTunings(sourceTuning, targetTuning);
    mappingMode = modeIn;
}

TunerController::TunerController(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTableMap> sourceMapping,
                                 std::shared_ptr<TuningTable> targetTuning, std::shared_ptr<TuningTableMap> targetMapping,
                                 Everytone::MappingMode modeIn, Everytone::MappingType typeIn)
    : mappingMode(modeIn), 
      mappingType(typeIn)
{
    setTunings(sourceTuning, sourceMapping, targetTuning, targetMapping);
}

TunerController::~TunerController()
{

}

void TunerController::setSourceTuning(std::shared_ptr<TuningTable> tuning)
{
    setSourceTuning(tuning, mapForTuning(tuning.get(), false), true);
}

void TunerController::setSourceTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping)
{
    setSourceTuning(tuning, mapping, true);
}

void TunerController::setTargetTuning(std::shared_ptr<TuningTable> tuning)
{
    setTargetTuning(tuning, mapForTuning(tuning.get(), true), true);
}

void TunerController::setTargetTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping)
{
    setTargetTuning(tuning, mapping, true);
}

void TunerController::setTunings(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTable> targetTuning)
{
    setTunings(sourceTuning, mapForTuning(sourceTuning.get(), false),
               targetTuning, mapForTuning(targetTuning.get(), true), true);
}

void TunerController::setTunings(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTableMap> sourceMapping,
    std::shared_ptr<TuningTable> targetTuning, std::shared_ptr<TuningTableMap> targetMapping)
{
    setTunings(sourceTuning, sourceMapping, targetTuning, targetMapping, true);
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
        tuning = currentTuningSource->setRootFrequency(root.frequency, true);
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
        tuning = currentTuningTarget->setRootFrequency(root.frequency, true);
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

void TunerController::loadSourceTuning(const TuningTable::Definition& definition, bool updateTuner)
{
    auto newTuning = std::make_shared<TuningTable>(definition);
    setSourceTuning(newTuning, mapForTuning(newTuning.get(), false), updateTuner);
}

void TunerController::loadTargetTuning(const TuningTable::Definition& definition, bool updateTuner)
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
                                 std::shared_ptr<TuningTable> targetTuning, std::shared_ptr<TuningTableMap> targetMapping, bool sendChangeMessages)
{
    setSourceTuning(sourceTuning, sourceMapping, false);
    setTargetTuning(targetTuning, targetMapping, false);

    updateCurrentTuner();

    // combine into one call?
    if (sendChangeMessages)
    {
        watchers.call(&Watcher::sourceTuningChanged, currentTuningSource);
        watchers.call(&Watcher::targetTuningChanged, currentTuningTarget);
    }
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

std::shared_ptr<TuningTableMap> TunerController::NewMappingFromTuning(const TuningTable* tuning, TuningTableMap::Root root, Everytone::MappingType mappingType)
{
    switch (mappingType)
    {
    case Everytone::MappingType::Linear:
        return MappedTuningTable::LinearMappingFromTuning(tuning, root);

    case Everytone::MappingType::Periodic:
        return MappedTuningTable::PeriodicMappingFromTuning(tuning, root);

    default:
        jassertfalse;
    }

    return nullptr;
};
