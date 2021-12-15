/*
  ==============================================================================

    Map_Test.h
    Created: 9 Nov 2021 11:30:32pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "Map_Test_Template.h"

class Map_Test_Generator
{
    OwnedArray<Map_Test<int>> tests;

public:

    Map_Test_Generator()
    {
        tests.add(NewMeantone12_31Test());
    }

    void addToTests(Array<UnitTest*> tests)
    {
        for (int i = 0; i < tests.size(); i++)
            tests.add(dynamic_cast<UnitTest*>(tests[i]));
    }

private:

    Map_Test<int>* NewMeantone12_31Test()
    {
        const int pattern[] = { 0, 3, 5, 8, 10, 13, 16, 18, 21, 23, 26, 29 };

        Map<int>::Definition definition = { 12, pattern, 31, 0, 0, 0 };

        const int expectedTable[] = {
            0, 3, 5, 8, 10, 13, 16, 18, 21, 23,
            26, 29, 31, 34, 36, 39, 41, 44, 47, 49,
            52, 54, 57, 60, 62, 65, 67, 70, 72, 75,
            78, 80, 83, 85, 88, 91, 93, 96, 98, 101,
            103, 106, 109, 111, 114, 116, 119, 122, 124, 127,
            129, 132, 134, 137, 140, 142, 145, 147, 150, 153,
            155, 158, 160, 163, 165, 168, 171, 173, 176, 178,
            181, 184, 186, 189, 191, 194, 196, 199, 202, 204,
            207, 209, 212, 215, 217, 220, 222, 225, 227, 230,
            233, 235, 238, 240, 243, 246, 248, 251, 253, 256,
            258, 261, 264, 266, 269, 271, 274, 277, 279, 282,
            284, 287, 289, 292, 295, 297, 300, 302, 305, 308,
            310, 313, 315, 318, 320, 323, 326, 328
        };

        const int altExpectedTable[] = {
            -95, -92, -90, -87, -85, -82, -79, -77, -74, -72, -69, -66,
            -64, -61, -59, -56, -54, -51, -48, -46, -43, -41, -38, -35,
            -33, -30, -28, -25, -23, -20, -17, -15, -12, -10, -7, -4,
            -2, 1, 3, 6, 8, 11, 14, 16, 19, 21, 24, 27,
            29, 32, 34, 37, 39, 42, 45, 47, 50, 52, 55, 58,
            60, 63, 65, 68, 70, 73, 76, 78, 81, 83, 86, 89,
            91, 94, 96, 99, 101, 104, 107, 109, 112, 114, 117, 120,
            122, 125, 127, 130, 132, 135, 138, 140, 143, 145, 148, 151,
            153, 156, 158, 161, 163, 166, 169, 171, 174, 176, 179, 182,
            184, 187, 189, 192, 194, 197, 200, 202, 205, 207, 210, 213,
            215, 218, 220, 223, 225, 228, 231, 233
        };

        Map_Test<int>::Parameters params =
        {
            "Meantone[12]31_Map",
            definition,
            128,
            expectedTable,
            60,
            60,
            altExpectedTable,
            1,
            altExpectedTable
        };

        return new Map_Test<int>("Int", params);
    }

    void doLinearConstructorTests()
    {
        /*auto defaultLinearMap = LinearMap();

        beginTest("Default Linear Map");
        auto defaultSize = defaultLinearMap.size();
        expect(defaultSize == 1, "Size is not 1: " + juce::String(defaultSize));

        auto defaultBase = defaultLinearMap.base();
        expect(defaultBase == 1, "Base is not 1: " + juce::String(defaultBase));

        const int expectedPattern = 0;

        Map<int>::Definition expectedDefinition = { 1, &expectedPattern, 1, 0, 0, 0 };

        auto defaultDefinition = defaultLinearMap.definition();
        expect(MapDefsAreEqual(expectedDefinition, defaultDefinition),
            "Map defs are not equal:\nExpected:\n" + MapDefToString(expectedDefinition) + "\n\nTested:\n" + MapDefToString(defaultDefinition));


        for (int i = -1000; i < 1000; i++)
        {
            bool testPassed = defaultLinearMap.at(i) == i;
            expect(testPassed, "Error testing Map values, Input: " + juce::String(i) + " Output: " + juce::String(defaultLinearMap.at(i)) + ", Expected: " + juce::String(i));
            if (!testPassed)
                return;

            testPassed = defaultLinearMap.periodsAt(i) == i;
            expect(testPassed, "Error testing Map periods, Input: " + juce::String(i) + " Output: " + juce::String(defaultLinearMap.periodsAt(i)) + ", Expected: " + juce::String(i));
            if (!testPassed)
                return;
        }

        auto linearMapAt10 = LinearMap(10, 1);

        for (int i = -1000; i < 1000; i++)
        {
            int expected = i - 10;
            bool testPassed = linearMapAt10.at(i) == expected;
            expect(testPassed, "Error testing Map with root at 10, Input: " + juce::String(i) + ", Output: " + juce::String(linearMapAt10.at(i)) + ", Expected: " + juce::String(expected));
            if (!testPassed)
                return;
        }


        auto linearMapPeriod2 = LinearMap(0, 2);

        for (int i = -1000; i < 1000; i++)
        {
            int expectedVal = i;
            bool testPassed = linearMapPeriod2.at(i) == expectedVal;
            expect(testPassed, "Error testing Map P2 Values, Input: " + juce::String(i) + " Output: " + juce::String(linearMapPeriod2.periodsAt(i)) + ", Expected: " + juce::String(expectedVal));
            if (!testPassed)
                return;

            int expectedPeriods = floor((double)i / 2);
            testPassed = linearMapPeriod2.periodsAt(i) == expectedPeriods;
            expect(testPassed, "Error testing Map P2 periods, Input: " + juce::String(i) + " Output: " + juce::String(linearMapPeriod2.periodsAt(i)) + ", Expected: " + juce::String(expectedPeriods));
            if (!testPassed)
                return;
        }*/
    }

};