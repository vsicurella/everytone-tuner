/*
  ==============================================================================

    Tuning_tests.h
    Created: 2 Dec 2021 6:54:30pm
    Author:  vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Tuning.h"

class Tuning_Test : public juce::UnitTest
{
private:

    static juce::String testErrorMessage(juce::String keyName, juce::var expected, juce::var actual)
    {
        juce::String msg = "Tuning." + keyName + " should equal " + expected.toString() + " but is " + actual.toString();
        return msg;
    }

    template <typename T>
    void expect_test(T expected, T actual, juce::String keyName)
    {
        expect(expected == actual, testErrorMessage(keyName, expected, actual));
    }

    void test_tuningMap(const juce::Array<double>& expected, const Keytographer::Map<double>* map, int start = 0)
    {
        int index;
        for (int i = 0; i < expected.size(); i++)
        {
            index = start + i;
            expect(expected[i] == map->at(index),
                   testErrorMessage("tuningMap at " + juce::String(index), expected[i], map->at(index)));
        }
    }

    void test_table(const juce::Array<double>& expected, const juce::Array<double>& table, juce::String tableName)
    {
        for (int i = 0; i < TUNING_TABLE_SIZE; i++)
        {
            expect(expected[i] == table[i], testErrorMessage(tableName + " at " + juce::String(i), expected[i], table[i]));
        }
    }

    void test_reference(Tuning::Reference expected, Tuning::Reference actual)
    {
        expect(expected.rootMidiNote == actual.rootMidiNote, testErrorMessage("reference.rootMidiNote", expected.rootMidiNote, actual.rootMidiNote));
        expect(expected.rootMidiChannel == actual.rootMidiChannel, testErrorMessage("reference.rootMidiChannel", expected.rootMidiChannel, actual.rootMidiChannel));
        expect(expected.rootFrequency == actual.rootFrequency, testErrorMessage("reference.rootFrequency", expected.rootFrequency, actual.rootFrequency));
    }

public:

    Tuning_Test() : juce::UnitTest("Tuning Test") {};

    void runTest() override
    {
        doDefaultTests();
    }

private:

    void doDefaultTests()
    {
        beginTest("Default Tuning Getters");
        Tuning tuning;

        auto definition = tuning.getDefinition();
        // expect(definition.transpose == 0,    testErrorMessage("definition.transpose", 0, definition.transpose));
        // expect(definition.name == "",        testErrorMessage("definition.name", "", definition.name));
        // expect(definition.description == "", testErrorMessage("definition.description", "", definition.description));
        expect_test(0, definition.transpose, "definition.transpose");
        expect_test(juce::String(), definition.name, "definition.name");
        expect_test(juce::String(), definition.description, "definition.description");

        auto reference = definition.reference;
        // expect(reference.rootMidiNote == 60, testErrorMessage("reference.rootMidiNote", 60, reference.rootMidiNote));
        // expect(reference.rootMidiChannel == 1, testErrorMessage("reference.rootMidiChannel", 1, reference.rootMidiChannel));
        // expect(reference.rootFrequency == 440.0, testErrorMessage("reference.rootFrequency", 440, reference.rootFrequency))
        expect_test(60, reference.rootMidiNote, "reference.rootMidiNote");
        expect_test(1, reference.rootMidiChannel, "reference.rootMidiChannel");
        expect_test(440.0, reference.rootFrequency, "reference.rootFrequency");
        DBG(reference.toString());

        auto getReference = tuning.getReference();
        DBG("testing Tuning.getReference()");
        test_reference(getReference, reference);

        expect_test(60, tuning.getRootIndex(), "getRootIndex()");
        expect_test(60, tuning.getRootMidiNote(), "getRootMidiNote()");
        expect_test(1, tuning.getRootMidiChannel(), "getRootMidiChannel()");
        expect_test(440.0, tuning.getRootFrequency(), "getRootFrequency()");

        expect_test(12, tuning.getTuningSize(), "getTuningSize()");
        expect_test(juce::String(), tuning.getName(), "getName()");
        expect_test(juce::String(), tuning.getDescription(), "getDescription()");


        beginTest("Default Tuning Tables");
        
        Tuning::IntervalDefinition defaultIntervalDefinition;
        juce::Array<double> expectedIntervalCents = { 0 };
        expectedIntervalCents.addArray(defaultIntervalDefinition.intervalCents);
        test_table(expectedIntervalCents, tuning.getIntervalCentsTable(), "getIntervalCentsTable()");

        juce::Array<double> expectedRatioTable = { 1.0, 1.0594630943592953, 1.122462048309373, 1.189207115002721, 1.2599210498948732, 1.3348398541700344, 1.4142135623730951, 1.4983070768766815, 1.5874010519681994, 1.681792830507429, 1.7817974362806785, 1.8877486253633868, 2.0 };
        test_table(expectedRatioTable, tuning.getIntervalRatioTable(), "getIntervalRatioTable");

        juce::Array<double> expectedFrequencyTable = { 8.1757989156, 8.661957218, 9.1770239974, 9.7227182413, 10.3008611535, 10.9133822323, 11.5623257097, 12.2498573744, 12.9782717994, 13.75, 14.5676175474, 15.4338531643, 16.3515978313, 17.3239144361, 18.3540479948, 19.4454364826, 20.6017223071, 21.8267644646, 23.1246514195, 24.4997147489, 25.9565435987, 27.5, 29.1352350949, 30.8677063285, 32.7031956626, 34.6478288721, 36.7080959897, 38.8908729653, 41.2034446141, 43.6535289291, 46.249302839, 48.9994294977, 51.9130871975, 55.0, 58.2704701898, 61.735412657, 65.4063913251, 69.2956577442, 73.4161919794, 77.7817459305, 82.4068892282, 87.3070578583, 92.4986056779, 97.9988589954, 103.826174395, 110.0, 116.5409403795, 123.470825314, 130.8127826503, 138.5913154884, 146.8323839587, 155.563491861, 164.8137784564, 174.6141157165, 184.9972113558, 195.9977179909, 207.65234879, 220.0, 233.081880759, 246.9416506281, 261.6255653006, 277.1826309769, 293.6647679174, 311.1269837221, 329.6275569129, 349.228231433, 369.9944227116, 391.9954359817, 415.3046975799, 440.0, 466.1637615181, 493.8833012561, 523.2511306012, 554.3652619537, 587.3295358348, 622.2539674442, 659.2551138257, 698.456462866, 739.9888454233, 783.9908719635, 830.6093951599, 880.0, 932.3275230362, 987.7666025122, 1046.5022612024, 1108.7305239075, 1174.6590716696, 1244.5079348883, 1318.5102276515, 1396.912925732, 1479.9776908465, 1567.981743927, 1661.2187903198, 1760.0, 1864.6550460724, 1975.5332050245, 2093.0045224048, 2217.461047815, 2349.3181433393, 2489.0158697766, 2637.020455303, 2793.825851464, 2959.9553816931, 3135.963487854, 3322.4375806396, 3520.0, 3729.3100921447, 3951.066410049, 4186.0090448096, 4434.92209563, 4698.6362866785, 4978.0317395533, 5274.0409106059, 5587.6517029281, 5919.9107633862, 6271.926975708, 6644.8751612791, 7040.0, 7458.6201842894, 7902.132820098, 8372.0180896192, 8869.8441912599, 9397.272573357, 9956.0634791066, 10548.0818212118, 11175.3034058561, 11839.8215267723, 12543.853951416 };
        test_table(expectedFrequencyTable, tuning.getFrequencyTable(), "getFrequencyTable");

        juce::Array<double> expectedMtsTable = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127 };
        test_table(expectedMtsTable, tuning.getMTSTable(), "getMtsTable()");


        beginTest("Default Tuning Setters");
        
        juce::String newName = "12edo";
        tuning.setName(newName);
        expect_test(newName, tuning.getName(), "setName()");

        juce::String newDescription = "Default tuning";
        tuning.setDescription(newDescription);
        expect_test(newDescription, tuning.getDescription(), "setDescription()");

        double newRootFrequency = 442.0;
        tuning.setRootFrequency(newRootFrequency);
        expect_test(newRootFrequency, tuning.getRootFrequency(), "setRootFrequency()");

        int newRootNote = 48;
        tuning.setRootMidiNote(newRootNote);
        expect_test(newRootNote, tuning.getRootMidiNote(), "setRootMidiNote()");

        int newRootChannel = 4;
        tuning.setRootMidiChannel(newRootChannel);
        expect_test(newRootChannel, tuning.getRootMidiChannel(), "setRootMidiChannel");

        auto changedReference = tuning.getReference();
        Tuning::Reference newReference = 
        {
            newRootNote,
            newRootChannel,
            newRootFrequency
        };
        DBG("testing new reference values");
        test_reference(newReference, changedReference);

        Tuning::Reference brandNewReference =
        {
            72,
            9,
            432.0
        };
        tuning.setReference(brandNewReference);
        expect_test(1096, tuning.getRootIndex(), "getRootIndex()");
        expect_test(72, tuning.getRootMidiNote(), "getRootMidiNote()");
        expect_test(9, tuning.getRootMidiChannel(), "getRootMidiChannel()");
        expect_test(432.0, tuning.getRootFrequency(), "getRootFrequency()");


    }
};
