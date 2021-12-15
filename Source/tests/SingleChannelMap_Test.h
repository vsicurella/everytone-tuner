/*
  ==============================================================================

    SingleChannelMap_Test.h
    Created: 9 Nov 2021 11:30:32pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "TestCommon.h"

namespace KeytographerTest
{

    class SingleChannelMap_Test : public juce::UnitTest
    {
    public:

        SingleChannelMap_Test() : juce::UnitTest("SingleChannelMap Test") {};

        void runTest() override
        {
            testMeantone31();

            doMeantone31Test();
        }

    private:

        void testMeantone31()
        {
            //const SingleChannelMap defaultMap;
            //beginTest("Default Map Size");

            //const int mapSize = defaultMap.getMapSize();
            //expect(mapSize == 12, "Default Map Size Failed: " + juce::String(mapSize));

            //beginTest("Default Map");

            //auto checkDefaultMap = [&](const SingleChannelMap& map)
            //{
            //    for (int i = 0; i < 12; i++)
            //        if (map.getMap()->at(i) != i)
            //            return false;

            //    return true;
            //};

            //juce::String mapString = standardMappingToString(defaultMap);
            //expect(checkDefaultMap(defaultMap), "Default Map Failed:\n" + mapString);

            //beginTest("Default Map Table");

            //auto checkDefaultMapTable = [&](int const* table)
            //{
            //    for (int i = 0; i < 128; i++)
            //        if (table[i] != i)
            //            return false;

            //    return true;
            //};

            //juce::String tableString = arrayToString(defaultMap.getTable(), 128);

            //expect(checkDefaultMapTable(defaultMap.getTable()), "Default Map Table Failed:\n" + tableString);
        }

        void doMeantone31Test()
        {
            const int mapSize = 12;

            //const int pattern[] = { 0, 3, 5, 8, 10, 13, 16, 18, 21, 23, 26, 29 };
            //auto meantone31map = SingleChannelMap(mapSize, pattern, 31, 60);

            //beginTest("Meantone31 Map Size");
            //expect(meantone31map.getMapSize() == mapSize, juce::String(mapSize));

            //beginTest("Meantone31 Table");
            //auto tableString = arrayToString(meantone31map.getTable(), 128);
            //const int expectedTable[] = {
            //    -95, -92, -90, -87, -85, -82, -79, -77, -74, -72, -69, -66,
            //    -64, -61, -59, -56, -54, -51, -48, -46, -43, -41, -38, -35,
            //    -33, -30, -28, -25, -23, -20, -17, -15, -12, -10, -7, -4,
            //    -2, 1, 3, 6, 8, 11, 14, 16, 19, 21, 24, 27,
            //    29, 32, 34, 37, 39, 42, 45, 47, 50, 52, 55, 58,
            //    60, 63, 65, 68, 70, 73, 76, 78, 81, 83, 86, 89,
            //    91, 94, 96, 99, 101, 104, 107, 109, 112, 114, 117, 120,
            //    122, 125, 127, 130, 132, 135, 138, 140, 143, 145, 148, 151,
            //    153, 156, 158, 161, 163, 166, 169, 171, 174, 176, 179, 182,
            //    184, 187, 189, 192, 194, 197, 200, 202, 205, 207, 210, 213,
            //    215, 218, 220, 223, 225, 228, 231, 233
            //};
            //auto testMap = [&](int const* table) {
            //    for (int i = 0; i < 128; i++)
            //        if (table[i] != expectedTable[i])
            //            return false;
            //    return true;
            //};
            //expect(testMap(meantone31map.getTable()), tableString);
        }
    };

}