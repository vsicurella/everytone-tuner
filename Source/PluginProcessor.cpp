/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "TuningHelpers.h"

#if RUN_MULTIMAPPER_TESTS
    #include "./tests/Map_Test_Generator.h"
    #include "./tests/MultichannelMap_Test.h"
    #include "./tests/Tuning_tests.h"
    #include "./tests/MidiNoteTuner_tests.h"
#endif


//==============================================================================
MultimapperAudioProcessor::MultimapperAudioProcessor()
    : tunerController(std::make_unique<TunerController>()),
      voiceController(std::make_unique<MidiVoiceController>(*tunerController)),
      voiceInterpolator(std::make_unique<MidiVoiceInterpolator>(*voiceController, Everytone::BendMode::Persistent)),

#ifndef JucePlugin_PreferredChannelConfigurations
     AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
#if JUCE_DEBUG
    logger = std::make_unique<MultimapperLog>();
    juce::Logger::setCurrentLogger(logger.get());
#endif


#if RUN_MULTIMAPPER_TESTS
    DBG("Running tests...");

    Map_Test_Generator mapTests;
    MultichannelMap_Test multichannelMapTest;
    FunctionalTuning_Test tuningTest;
    MidiNoteTuner_Test midiNoteTunerTest;

    auto tests = juce::Array<juce::UnitTest*>();
    mapTests.addToTests(tests);
    tests.add(&multichannelMapTest);
    tests.add(&tuningTest);
    tests.add(&midiNoteTunerTest);

    juce::UnitTestRunner tester;
    tester.runTests(tests);

    //juce::Timer::callAfterDelay(500, [&]() { this->testMidi(); });
#endif
}

MultimapperAudioProcessor::~MultimapperAudioProcessor()
{
    juce::Logger::setCurrentLogger(nullptr);
    logger = nullptr;

    voiceController = nullptr;
    tunerController = nullptr;
}

//==============================================================================
const juce::String MultimapperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MultimapperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MultimapperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MultimapperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MultimapperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MultimapperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MultimapperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MultimapperAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MultimapperAudioProcessor::getProgramName (int index)
{
    return {};
}

void MultimapperAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MultimapperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void MultimapperAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MultimapperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MultimapperAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }

    tuneMidiBuffer(midiMessages);
}

//==============================================================================
bool MultimapperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MultimapperAudioProcessor::createEditor()
{
    return new MultimapperAudioProcessorEditor (*this);
}

//==============================================================================
void MultimapperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ValueTree state(Everytone::ID::State);

    //auto sourceNode = tuningToValueTree(*midiBrain->getTuningSource(), Everytone::ID::TuningSource);
    //state.addChild(sourceNode, -1, nullptr);
    
    auto targetNode = mappedTuningToValueTree(tunerController->readTuningTarget(), Everytone::ID::TuningTarget);
    state.addChild(targetNode, 0, nullptr);

    auto optionsNode = options().toValueTree();
    state.addChild(optionsNode, -1, nullptr);

    juce::Logger::writeToLog("Saved:\n" + state.toXmlString());

    juce::MemoryOutputStream stream(destData, false);
    state.writeToStream(stream);
}

void MultimapperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::MemoryBlock       buffer(data, (size_t)sizeInBytes);
    juce::MemoryInputStream stream(buffer, false);
    
    auto state = juce::ValueTree::readFromStream(stream);

    juce::Logger::writeToLog("Loading:\n" + state.toXmlString());

    //auto sourceTree = state.getChildWithName(Everytone::ID::TuningSource);
    //if (sourceTree.isValid())
    //{
    //    auto source = parseTuningValueTree(state.getChild(0));
    //    loadTuningSource(source);
    //}

    // For now, always load standard source mapping
    auto sourceTuning = FunctionalTuning::StandardTuning();
    auto sourceMapping = std::make_shared<TuningTableMap>(TuningTableMap::StandardMappingDefinition());
    MappedTuningTable::FrequencyReference sourceReference;

    std::shared_ptr<TuningTable> targetTuning;
    std::shared_ptr<TuningTableMap> targetMapping;
    MappedTuningTable::FrequencyReference targetReference;

    auto targetTree = state.getChildWithName(Everytone::ID::TuningTarget);
    if (targetTree.isValid())
    {
        auto tuningTree = targetTree.getChildWithName(Everytone::ID::Tuning);
        auto parsedTuning = constructTuningFromValueTree(tuningTree);
        targetTuning = (parsedTuning != nullptr) ? parsedTuning
                                                 : sourceTuning;

        auto mapTree = targetTree.getChildWithName(Everytone::ID::TuningTableMidiMap);
        targetMapping = constructTuningTableMapFromValueTree(mapTree);

        if (targetTree.hasProperty(Everytone::ID::ReferenceMidiChannel))
            targetReference.midiChannel = (int)targetTree[Everytone::ID::ReferenceMidiChannel];

        if (targetTree.hasProperty(Everytone::ID::ReferenceMidiNote))
            targetReference.midiNote = (int)targetTree[Everytone::ID::ReferenceMidiNote];
    }

    auto optionsTree = state.getChildWithName(Everytone::ID::Options);
    Everytone::Options loadedOptions;
    if (optionsTree.isValid())
    {
        loadedOptions = Everytone::Options::fromValueTree(optionsTree);
    }

    if (targetTuning == nullptr)
        targetTuning = sourceTuning;

    if (targetMapping == nullptr)
        tunerController->setTunings(sourceTuning, sourceReference, targetTuning, targetReference);
    else
        tunerController->setTunings(sourceTuning, sourceMapping, sourceReference, targetTuning, targetMapping, targetReference);

    options(loadedOptions);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultimapperAudioProcessor();
}

MultimapperLog* MultimapperAudioProcessor::getLog() const
{
    return logger.get();
}

void MultimapperAudioProcessor::tuneMidiBuffer(juce::MidiBuffer& buffer)
{
    // Update active voices
    juce::MidiBuffer interpolationMessages;
    int interpolationSample = 0;

    auto voiceTargets = voiceInterpolator->getAndClearVoiceTargets();
    for (auto voice : voiceTargets)
    {
        if (voice.getAssignedChannel() >= 0)
        {
            interpolationMessages.addEvent(voice.getPitchbend(), interpolationSample++);
        }
        else
            jassertfalse;
    }

    // Process new messages
    juce::MidiBuffer processedBuffer;
    int processedSample = 0;

    for (auto metadata : buffer)
    {
        auto msg = metadata.getMessage();
        auto status = msg.getRawData()[0] & 0xF0;

        switch (status)
        {
            case 0x80: // Note Off
            {
                auto voice = voiceController->removeVoice(msg);
                if (voice.isValid())
                    voice.mapMidiMessage(msg);
                else
                {
                    jassertfalse;
                }

                break;
            }

            case 0x90: // Note On
            {
                if (msg.isNoteOn())
                {
                    auto voice = voiceController->getVoice(msg);
                    if (voice == nullptr)
                        continue;

                    voice->mapMidiMessage(msg);

                    auto pbmsg = voice->getPitchbend();
                    if (pbmsg.getPitchWheelValue() != 8192)
                    {
                        processedBuffer.addEvent(pbmsg, processedSample++);
                    }
                }
                break;
            }

            case 0xA0: // Aftertouch
            {
                auto voice = voiceController->getVoice(msg);
                if (voice == nullptr)
                    continue;            
                
                voice->mapMidiMessage(msg);
                break;
            }

            default:
                break;
        }

        processedBuffer.addEvent(msg, processedSample++);
    }

    MidiBuffer priorityBuffer;
    int priorityBufferSize = voiceController->serveNotePriorityMessages(priorityBuffer);
    
    // Insert in order: Interpolation buffer, NotePriority buffer, Processed buffer

    MidiBuffer combinedBuffer;
    combinedBuffer.addEvents(interpolationMessages, 0, interpolationSample, 0);
    combinedBuffer.addEvents(priorityBuffer, 0, priorityBufferSize, interpolationSample);
    combinedBuffer.addEvents(processedBuffer, 0, processedSample, interpolationSample + priorityBufferSize);

    buffer.swapWith(combinedBuffer);
}

//void MultimapperAudioProcessor::testMidi()
//{
//    juce::MidiBuffer buffer;
//    int sample = 0;
//    // Note on + off for each note in channel
//    for (int ch = 1; ch <= 1; ch++)
//        for (int n = 0; n < 128; n++)
//        {
//            auto msg = juce::MidiMessage::noteOn(ch, n, (juce::uint8)100);
//            buffer.addEvent(msg, sample++);
//            msg = juce::MidiMessage::noteOff(ch, n);
//            buffer.addEvent(msg, sample++);
//        }
//
//    auto audioDummy = juce::AudioSampleBuffer();
//    processBlock(audioDummy, buffer);
//}

Everytone::Options MultimapperAudioProcessor::options() const
{
    return Everytone::Options
    {
        tunerController->getMappingMode(),
        tunerController->getMappingType(),
        voiceController->getChannelMode(),
        voiceController->getMpeZone(),
        Everytone::MidiMode::Mono,
        voiceController->getNotePriority(),
        voiceInterpolator->getBendMode(),
        voiceController->getVoiceLimit(),
        tunerController->getPitchbendRange()
    };
}

void MultimapperAudioProcessor::loadTuningSource(const CentsDefinition& tuningDefinition)
{
    auto newSource = std::make_shared<FunctionalTuning>(tuningDefinition);
    tunerController->setSourceTuning(newSource);
}

void MultimapperAudioProcessor::loadTuningTarget(const CentsDefinition& tuningDefinition)
{
    auto newTarget = std::make_shared<FunctionalTuning>(tuningDefinition, true);
    tunerController->setTargetTuning(newTarget);
}

void MultimapperAudioProcessor::setTuningSource(std::shared_ptr<TuningTable> sourceTuning)
{
    tunerController->setSourceTuning(sourceTuning);
}

void MultimapperAudioProcessor::setTuningTarget(std::shared_ptr<TuningTable> targetTuning)
{
    tunerController->setTargetTuning(targetTuning);
}

void MultimapperAudioProcessor::setTargetTuningReference(MappedTuningTable::FrequencyReference reference)
{
    tunerController->setTargetReference(reference);
}

void MultimapperAudioProcessor::setTargetTuningRootFrequency(double frequency)
{
    tunerController->setTargetRootFrequency(frequency);
}

void MultimapperAudioProcessor::setTargetMappingRoot(TuningTableMap::Root root)
{
    tunerController->setTargetMapRoot(root);
}

void MultimapperAudioProcessor::setTargetMappedTuningRoot(MappedTuningTable::Root root)
{
    tunerController->setTargetMappedTuningRoot(root);
}

//void MultimapperAudioProcessor::loadNoteMapping(const TuningTableMap& map)
//{
//    tunerController->setNoteMapping(&map);
//}

void MultimapperAudioProcessor::autoMappingType(Everytone::MappingType type)
{
    tunerController->setMappingType(type);
}

void MultimapperAudioProcessor::mappingMode(Everytone::MappingMode mode)
{
    tunerController->setMappingMode(mode);
}

void MultimapperAudioProcessor::channelMode(Everytone::ChannelMode mode)
{
    voiceController->setChannelMode(mode);
}

void MultimapperAudioProcessor::mpeZone(Everytone::MpeZone zone)
{
    voiceController->setMpeZone(zone);
}

void MultimapperAudioProcessor::voiceLimit(int voiceLimit)
{
    voiceController->setVoiceLimit(voiceLimit);
}

void MultimapperAudioProcessor::pitchbendRange(int pitchbendRange)
{
    tunerController->setPitchbendRange(pitchbendRange);
}

void MultimapperAudioProcessor::bendMode(Everytone::BendMode bendMode)
{
    voiceInterpolator->setBendMode(bendMode);
}

void MultimapperAudioProcessor::options(Everytone::Options optionsIn)
{
    autoMappingType(optionsIn.mappingType);
    mappingMode(optionsIn.mappingMode);
    channelMode(optionsIn.channelMode);
    bendMode(optionsIn.bendMode);
    voiceLimit(optionsIn.voiceLimit);
    pitchbendRange(optionsIn.pitchbendRange);
}
