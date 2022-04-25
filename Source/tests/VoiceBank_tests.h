/*
  ==============================================================================

    VoiceBank_tests.h
    Created: 24 Apr 2022 9:12:33pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include "TestsCommon.h"
#include "../voices/VoiceBank.h"

class VoiceBank_Test : public EverytoneTunerUnitTest
{

private:

    struct TestNote
    {
        int note = 0;
        int channel = 1;
        int expectedChannel = 0;
        int iterationAdded = 0;
        juce::MidiMessage msg;

        juce::String toString()
        {
            return juce::String("Test Note #") + juce::String(iterationAdded) 
                + " (Nt " + juce::String(note) 
                + " , Ch " + juce::String(channel)
                + ") expected on channel " + juce::String(expectedChannel)
                ;
        }
    };

    bool testNoteExists(juce::Array<TestNote>& arr, int note, int channel)
    {
        for (auto n : arr)
            if (note == n.note && channel == n.channel)
                return true;
        return false;
    }

    int nextExpectedChannel(const VoiceBank& bank, juce::Array<TestNote>& testNotes)
    {
        auto channelMode = bank.getChannelMode();
        auto mpeZone = bank.getMpeZone();

        if (channelMode == Everytone::ChannelMode::Monophonic)
            return bank.getLastAssignedChannel();


        juce::Array<int> allChannels;
        for (int i = 0; i < 16; i++)
        {
            if (mpeZone == Everytone::MpeZone::Lower && i == 0)
                continue;

            if (mpeZone == Everytone::MpeZone::Upper && i == 15)
                continue;

            allChannels.add(i + 1);
        }

        juce::Array<int> availableChannels = allChannels;
        for (auto note : testNotes)
            availableChannels.removeFirstMatchingValue(note.expectedChannel);

        if (availableChannels.size() == 0)
            return 0;

        switch (channelMode)
        {
            case Everytone::ChannelMode::FirstAvailable:
                return availableChannels[0];

            case Everytone::ChannelMode::RoundRobin:
            {
                int lastAssigned = bank.getLastAssignedChannel();
                if (lastAssigned == 16)
                    lastAssigned = 0;

                for (auto ch : availableChannels)
                    if (ch > lastAssigned)
                        return ch;

                return availableChannels[0];
            }

            default:
                jassertfalse;
        }

        return 0;
    }

    void testVoice(TestNote& note, const MidiVoice* voice)
    {
        auto noteStr = note.toString();
        expect(voice != nullptr, noteStr + " was not found.");
        if (voice == nullptr)
            return;

        expect(voice->getMidiNote() == note.note, noteStr + " voice has midi note " + juce::String(voice->getMidiNote()));
        expect(voice->getMidiChannel() == note.channel, noteStr + " voice has midi channel " + juce::String(voice->getMidiChannel()));
        expect(voice->getAssignedChannel() == note.expectedChannel, noteStr + " was assigned to voice channel " + juce::String(voice->getAssignedChannel()));
    }

    void testBank(const VoiceBank& bank, const juce::Array<TestNote>& notes)
    {
        for (auto note : notes)
        {
            auto voice = bank.readVoice(note.msg);
            testVoice(note, voice);
        }
    }

public:

    VoiceBank_Test() : EverytoneTunerUnitTest("VoiceBank") {}

    void runTest() override
    {
        testChannelModes();
    }

    void testChannelMode(Everytone::ChannelMode channelMode, Everytone::MpeZone mpeZone, VoiceBank* voiceBank=nullptr)
    {
        juce::Random r;

        std::unique_ptr<VoiceBank> uniqueVoiceBank;

        if (voiceBank == nullptr)
        {
            uniqueVoiceBank.reset(new VoiceBank(channelMode, mpeZone));
            voiceBank = uniqueVoiceBank.get();
        }

        const int testLength = 1000;
        const int checkBankOdds = 1;
        const int noteOffOdds = 3;

        beginTest("ChannelMode::" + Everytone::getName(channelMode) + " and MpeZone::" + Everytone::getName(mpeZone));
        
        juce::Array<TestNote> notesOn;

        int i = 0;
        int lastExpectedChannel = 16;
        int notesRemoved = 0;
        while (i < testLength)
        {
            int note = r.nextInt(127);
            int channel = r.nextInt(juce::Range<int>(1, 16));

            bool exists = testNoteExists(notesOn, note, channel);
            if (exists) // prevent duplicate notes
                continue;

            auto noteOn = juce::MidiMessage::noteOn(channel, note, (juce::uint8)100);

            int expectedChannel = nextExpectedChannel(*voiceBank, notesOn);
            if (expectedChannel == 0)
            {
                jassertfalse;
                nextExpectedChannel(*voiceBank, notesOn);
            }

            auto testNote = TestNote{ note, channel, expectedChannel, i, noteOn };

            auto newVoice = voiceBank->getVoice(noteOn);
            notesOn.add(testNote);
            testVoice(testNote, newVoice);

            //if (channelMode == Everytone::ChannelMode::RoundRobin)
            //    DBG("Notes on: " + juce::String(notesOn.size()) + ", Notes Removed: " + juce::String(notesRemoved));

            lastExpectedChannel = expectedChannel;

            if (notesOn.size() > 0)
            {
                int makeNoteOff = r.nextInt(noteOffOdds) + noteOffOdds;
                if (makeNoteOff == noteOffOdds || notesOn.size() == 15)
                {
                    int testNoteIndex = r.nextInt(notesOn.size());
                    auto testNote = notesOn[testNoteIndex];
                    auto noteOff = juce::MidiMessage::noteOff(testNote.channel, testNote.note);
                    voiceBank->removeVoice(noteOff);
                    notesOn.remove(testNoteIndex);
                    notesRemoved++;
                }
            }

            testBank(*voiceBank, notesOn);

            i++;
        }

        // finish notes off
        for (auto note : notesOn)
        {
            auto noteOff = juce::MidiMessage::noteOff(note.channel, note.note);
            voiceBank->removeVoice(noteOff);
        }

        notesOn.clear();
        testBank(*voiceBank, notesOn);

        voiceBank = nullptr;
    }

    void testChannelModes()
    {
        testChannelMode(Everytone::ChannelMode::FirstAvailable, Everytone::MpeZone::Omnichannel);
        testChannelMode(Everytone::ChannelMode::FirstAvailable, Everytone::MpeZone::Lower);
        testChannelMode(Everytone::ChannelMode::FirstAvailable, Everytone::MpeZone::Upper);

        testChannelMode(Everytone::ChannelMode::RoundRobin, Everytone::MpeZone::Omnichannel);
        testChannelMode(Everytone::ChannelMode::RoundRobin, Everytone::MpeZone::Lower);
        testChannelMode(Everytone::ChannelMode::RoundRobin, Everytone::MpeZone::Upper);
    }
};
