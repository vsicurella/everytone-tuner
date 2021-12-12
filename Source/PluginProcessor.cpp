/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "TuningHelpers.h"

//==============================================================================
MultimapperAudioProcessor::MultimapperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    midiBrain.reset(new MidiBrain());

#if JUCE_DEBUG
    logger.reset(new MultimapperLog([&](juce::StringRef msg) { dbgLog += msg + '\n'; }));
    juce::Logger::setCurrentLogger(logger.get());

    #if RUN_MULTIMAPPER_TESTS
    DBG("Running tests...");

    Tuning_Test tuningTest;

    auto tests = juce::Array<juce::UnitTest*>();
    tests.add(&tuningTest);

    juce::UnitTestRunner tester;
    tester.runTests(tests);

    //juce::Timer::callAfterDelay(500, [&]() { this->testMidi(); });
    #endif
#endif
}

MultimapperAudioProcessor::~MultimapperAudioProcessor()
{
    juce::Logger::setCurrentLogger(nullptr);
    logger = nullptr;
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

    midiBrain->processMidi(midiMessages);
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
    juce::ValueTree state(Multimapper::ID::State);

    //auto sourceNode = tuningToValueTree(*midiBrain->getTuningSource(), Multimapper::ID::TuningSource);
    //state.addChild(sourceNode, -1, nullptr);
    
    auto targetNode = tuningToValueTree(*midiBrain->getTuningTarget(), Multimapper::ID::TuningTarget);
    state.addChild(targetNode, -1, nullptr);

    juce::MemoryOutputStream stream(destData, false);
    state.writeToStream(stream);
}

void MultimapperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::MemoryBlock       buffer(data, (size_t)sizeInBytes);
    juce::MemoryInputStream stream(buffer, false);
    
    auto state = juce::ValueTree::readFromStream(juce::MemoryInputStream(data, (size_t)sizeInBytes, false));

    //auto sourceTree = state.getChildWithName(Multimapper::ID::TuningSource);
    //if (sourceTree.isValid())
    //{
    //    auto source = parseTuningValueTree(state.getChild(0));
    //    loadTuningSource(source);
    //}

    auto targetTree = state.getChildWithName(Multimapper::ID::TuningTarget);
    if (targetTree.isValid())
    {
        auto target = parseTuningValueTree(targetTree);
        loadTuningTarget(target);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultimapperAudioProcessor();
}

juce::String MultimapperAudioProcessor::getLog() const
{
    return dbgLog;
}

void MultimapperAudioProcessor::testMidi()
{
    juce::MidiBuffer buffer;
    int sample = 0;
    // Note on + off for each note in channel
    for (int ch = 1; ch <= 1; ch++)
        for (int n = 0; n < 128; n++)
        {
            auto msg = juce::MidiMessage::noteOn(ch, n, (juce::uint8)100);
            buffer.addEvent(msg, sample++);
            msg = juce::MidiMessage::noteOff(ch, n);
            buffer.addEvent(msg, sample++);
        }

    midiBrain->processMidi(buffer);

    //int ch = 1;
    //int n = 0;
    //for (auto metadata : buffer)
    //{
    //    auto msg = metadata.getMessage();
    //    if (msg.isNoteOn())
    //    {
    //        jassert(msg.getChannel() == ch);
    //        jassert(msg.getNoteNumber() == n);

    //        n++;
    //        if (n == 128)
    //        {
    //            ch++;
    //            n = 0;
    //        }
    //    }
    //}
}

void MultimapperAudioProcessor::loadTuningSource(const Tuning& tuning)
{
    midiBrain->setTuningSource(tuning);
}

void MultimapperAudioProcessor::loadTuningTarget(const Tuning& tuning)
{
    midiBrain->setTuningTarget(tuning);
    tuningMapHelper.createTuningMap(tuning);
    juce::Logger::writeToLog("Loaded new tuning: " + tuning.getDescription());

    /* TODO if auto mapping */
    loadNoteMapping(tuningMapHelper.getTuningMap());
}

void MultimapperAudioProcessor::loadNoteMapping(const Keytographer::TuningTableMap& map)
{
    midiBrain->setNoteMap(map);
    juce::Logger::writeToLog("Loaded new mapping");
}

const Tuning* MultimapperAudioProcessor::activeSourceTuning() const
{
    return midiBrain->getTuningSource();
}

const Tuning* MultimapperAudioProcessor::activeTargetTuning() const
{
    return midiBrain->getTuningTarget();
}
