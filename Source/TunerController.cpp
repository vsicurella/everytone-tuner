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

void TunerController::setSourceTuning(std::shared_ptr<TuningTable> tuning, bool setReference, MappedTuningTable::FrequencyReference reference)
{
    if (setReference)
        sourceReference = reference;

    auto mapping = mapForTuning(tuning.get(), false);
    setSourceTuning(tuning, mapping, true);
}

void TunerController::setSourceTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping)
{
    setSourceTuning(tuning, mapping, true);
}

void TunerController::setSourceTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping, MappedTuningTable::FrequencyReference sourceReferenceIn)
{
    sourceReference = sourceReferenceIn;
    setSourceTuning(tuning, mapping, true);
}

void TunerController::setTargetTuning(std::shared_ptr<TuningTable> tuning, bool setReference, MappedTuningTable::FrequencyReference reference)
{
    if (setReference)
        targetReference = reference;

    auto mapping = mapForTuning(tuning.get(), true);
    setTargetTuning(tuning, mapping, true);
}

void TunerController::setTargetTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping)
{
    setTargetTuning(tuning, mapping, true);
}

void TunerController::setTargetTuning(std::shared_ptr<TuningTable> tuning, std::shared_ptr<TuningTableMap> mapping, MappedTuningTable::FrequencyReference targetReferenceIn)
{
    targetReference = targetReferenceIn;
    setTargetTuning(tuning, mapping, true);
}

void TunerController::setTunings(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTable> targetTuning)
{
    auto newSourceMapping = mapForTuning(sourceTuning.get(), false);
    auto newTargetMapping = mapForTuning(targetTuning.get(), true);
    setTunings(sourceTuning, newSourceMapping,
               targetTuning, newTargetMapping, true);
}

void TunerController::setTunings(std::shared_ptr<TuningTable> sourceTuning, MappedTuningTable::FrequencyReference sourceReferenceIn,
                                 std::shared_ptr<TuningTable> targetTuning, MappedTuningTable::FrequencyReference targetReferenceIn)
{
    sourceReference = sourceReferenceIn;
    targetReference = targetReferenceIn;
    setTunings(sourceTuning, targetTuning);
}

void TunerController::setTunings(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTableMap> sourceMapping,
                                 std::shared_ptr<TuningTable> targetTuning, std::shared_ptr<TuningTableMap> targetMapping)
{
    setTunings(sourceTuning, sourceMapping, targetTuning, targetMapping, true);
}

void TunerController::setTunings(std::shared_ptr<TuningTable> sourceTuning, std::shared_ptr<TuningTableMap> sourceMapping, MappedTuningTable::FrequencyReference sourceReferenceIn,
                                 std::shared_ptr<TuningTable> targetTuning, std::shared_ptr<TuningTableMap> targetMapping, MappedTuningTable::FrequencyReference targetReferenceIn)
{
    sourceReference = sourceReferenceIn;
    targetReference = targetReferenceIn;
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

void TunerController::setSourceReference(MappedTuningTable::FrequencyReference reference)
{
    auto root = MappedTuningTable::Root { reference, currentTuningSource->getRootFrequency(), sourceMapRoot };
    setSourceMappedTuningRoot(root);
}

void TunerController::setTargetReference(MappedTuningTable::FrequencyReference reference)
{
    auto root = MappedTuningTable::Root { reference, currentTuningTarget->getRootFrequency(), targetMapRoot };
    setTargetMappedTuningRoot(root);
}

void TunerController::setSourceRootFrequency(double frequency)
{
    MappedTuningTable::Root mappedRoot =
    {
        sourceReference,
        frequency,
        sourceMapRoot
    };
    setSourceMappedTuningRoot(mappedRoot);
}

void TunerController::setTargetRootFrequency(double frequency)
{
    MappedTuningTable::Root mappedRoot =
    {
        targetReference,
        frequency,
        targetMapRoot
    };
    setTargetMappedTuningRoot(mappedRoot);
}

void TunerController::setSourceMapRoot(TuningTableMap::Root root)
{
    MappedTuningTable::Root mappedRoot =
    {
        sourceReference,
        currentTuningSource->getRootFrequency(),
        { root.midiChannel, root.midiNote }
    };
    setSourceMappedTuningRoot(mappedRoot);
}

void TunerController::setTargetMapRoot(TuningTableMap::Root root)
{
    MappedTuningTable::Root mappedRoot =
    {
        targetReference,
        currentTuningTarget->getRootFrequency(),
        { root.midiChannel, root.midiNote }
    };
    setTargetMappedTuningRoot(mappedRoot);
}

void TunerController::setSourceMappedTuningRoot(MappedTuningTable::Root root)
{
    // These would be great elsewhere, but at the moment TuningTableMap can't recreate itself with a different root

    bool tuningChanged = currentTuningSource->getRootFrequency() != root.frequency;
    bool mappingChanged = root.tuningReference.isInvalid()
                       || sourceMapRoot != root.mapping
                       || currentTuningSource->getFrequencyReference() != root.tuningReference;

    sourceMapRoot = root.mapping;
    sourceReference = root.tuningReference;

    auto tuning = currentTuningSource->shareTuning();
    if (tuningChanged)
    {
        auto currentTuningIndex = tuning->getRootIndex();
        tuning = currentTuningSource->setRootFrequency(root.frequency, true);
        mappingChanged = currentTuningIndex != tuning->getRootIndex();
    }

    auto mapping = currentTuningSource->shareMapping();
    if (mappingChanged)
        mapping = mapForTuning(tuning.get(), false);

    if (mappingChanged || tuningChanged)
        setSourceTuning(tuning, mapping);
}

void TunerController::setTargetMappedTuningRoot(MappedTuningTable::Root root)
{
    bool tuningChanged = currentTuningTarget->getRootFrequency() != root.frequency;
    bool mappingChanged = root.tuningReference.isInvalid() 
                       || targetMapRoot != root.mapping 
                       || currentTuningTarget->getFrequencyReference() != root.tuningReference;

    targetMapRoot = root.mapping;
    targetReference = root.tuningReference;

    auto tuning = currentTuningTarget->shareTuning();
    if (tuningChanged)
    {
        auto currentTuningIndex = tuning->getRootIndex();
        tuning = currentTuningTarget->setRootFrequency(root.frequency, true);
        mappingChanged = currentTuningIndex != tuning->getRootIndex();
    }

    auto mapping = currentTuningTarget->shareMapping();
    if (mappingChanged)
        mapping = mapForTuning(tuning.get(), true);

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

void TunerController::setSourceTuning(std::shared_ptr<TuningTable>& tuning, std::shared_ptr<TuningTableMap>& mapping, bool updateTuner)
{
    auto mappedSource = std::make_shared<MappedTuningTable>(tuning, mapping);
    setSource(mappedSource, updateTuner);
}

void TunerController::setSource(std::shared_ptr<MappedTuningTable>& mappedTuning, bool updateTuner)
{
    sourceReference = mappedTuning->getFrequencyReference();
    sourceMapRoot = mappedTuning->getMappingRoot();
    currentTuningSource = mappedTuning;
    juce::Logger::writeToLog("Loaded new source tuning: " + currentTuningSource->getDescription());

    if (updateTuner)
    {
        updateCurrentTuner();
        watchers.call(&Watcher::sourceTuningChanged, currentTuningSource);
    }
}

void TunerController::setTargetTuning(std::shared_ptr<TuningTable>& tuning, std::shared_ptr<TuningTableMap>& mapping, bool updateTuner)
{
    auto mappedTarget = std::make_shared<MappedTuningTable>(tuning, mapping, targetReference);
    setTarget(mappedTarget, updateTuner);
}

void TunerController::setTarget(std::shared_ptr<MappedTuningTable>& mappedTuning, bool updateTuner)
{
    targetReference = mappedTuning->getFrequencyReference();
    targetMapRoot = mappedTuning->getMappingRoot();
    currentTuningTarget = mappedTuning;
    juce::Logger::writeToLog("Loaded new target tuning: " + currentTuningSource->getDescription());

    if (updateTuner)
    {
        updateCurrentTuner();
        watchers.call(&Watcher::targetTuningChanged, currentTuningTarget);
    }
}

void TunerController::setTunings(std::shared_ptr<TuningTable>& sourceTuning, std::shared_ptr<TuningTableMap>& sourceMapping,
                                 std::shared_ptr<TuningTable>& targetTuning, std::shared_ptr<TuningTableMap>& targetMapping, bool sendChangeMessages)
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
