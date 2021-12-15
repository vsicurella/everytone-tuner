/*
  ==============================================================================

    Map_Test.h
    Created: 9 Nov 2021 11:30:32pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "TestsCommon.h"


template <typename T>
class Map_Test : public MultimapperUnitTest
{
private:

    void testMap(int size, const T* expected, const Map<T>& map, String name)
    {
        for (int i = 0; i < size; i++)
            expect_exact<T>(expected[i], map.at(i), name + " map at " + String(i));
    };

    String MapDefToString(typename Map<T>::Definition def)
    {
        String str = "Size: " + String(def.mapSize);
        str += "\nPattern   : " + arrayToString<T>(def.pattern, def.mapSize, false);
        str += "\nBase      : " + String(def.patternBase);
        str += "\nPat. Root : " + String(def.patternRootIndex);
        str += "\nMap Root  : " + String(def.mapRootIndex);
        str += "\nTranspose : " + String(def.transpose);
        return str;
    }

public:

    struct Parameters
    {
        String testName;
        typename Map<T>::Definition definition;
        const int testTableSize;
        const T* expectedTable = nullptr;

        int altMapRoot;
        T altTranspose;
        const T* altExpectedTable = nullptr;

        int altPatternRootIndex = 1;
        const T* altPatternRootExpectedTable = nullptr;
    };

private:

    Parameters params;

public:

    Map_Test(String nameOfType, Parameters testParams) 
        : MultimapperUnitTest("Map " + nameOfType),
          params(testParams) {}

    void runTest() override
    {
        auto definition = params.definition;
        auto map = Map<T>(definition);

        beginTest("Getters");
        expect_exact<T>(definition.mapSize, map.size(), "mapSize");
        test_table<T, std::vector<T>>(definition.mapSize, definition.pattern, map.pattern(), "pattern");
        expect_exact<T>(definition.patternBase, map.base(), "base");

        auto expectedPatternRoot = mod(definition.patternRootIndex, definition.mapSize);
        expect_equals<T>(expectedPatternRoot, map.patternRoot(), "patternRoot");

        auto patternRootValue = map.patternAt(map.patternRoot());
        expect_equals<T>(definition.pattern[expectedPatternRoot], patternRootValue, "patternRoot value");
        expect_equals<T>(definition.mapRootIndex, map.mapRoot(), "mapRoot");

        auto mapRootValue = map.patternAt(map.patternRoot());
        expect_equals<T>(definition.pattern[expectedPatternRoot], patternRootValue, "mapRoot value");

        expect_equals<T>(definition.transpose, map.transposition(), "transpose");

        auto redefinition = map.definition();
        expect(definition == redefinition, "Map defs are not equal:\nOG:\n" + MapDefToString(definition) + "\n\nNew:\n" + MapDefToString(redefinition));

        // Main map test
        testMap(params.testTableSize, params.expectedTable, map, params.testName);


        beginTest("Setters");
        T newBase = 0;
        Array<T> base0Table;
        base0Table.resize(params.testTableSize);
        for (int i = 0; i < params.testTableSize; i++)
            base0Table.set(i, definition.pattern[i % definition.mapSize]);

        map.setBase(newBase);
        testMap(params.testTableSize, base0Table.data(), map, params.testName + "base change");

        // reset defaultBase
        map.setBase(definition.patternBase);
        testMap(params.testTableSize, params.expectedTable, map, params.testName + "base reset");

        T transposeTest = 3;
        Array<T> mapTransposed;
        mapTransposed.resize(params.testTableSize);
        for (int i = 0; i < params.testTableSize; i++)
            mapTransposed.set(i, params.expectedTable[i] + transposeTest);

        map.setTranspose(transposeTest);
        testMap(params.testTableSize, mapTransposed.data(), map, params.testName + "transposed");

        map.setMapRoot(params.altMapRoot);
        map.setTranspose(params.altTranspose);
        testMap(params.testTableSize, params.altExpectedTable, map, params.testName + "root & transpose change");

        int altPatternRoot = 1;
        map.setPatternRoot(altPatternRoot);

        const T* altPatternRootTable = params.altExpectedTable + 1;
        testMap(params.testTableSize - 1, altPatternRootTable, map, params.testName + "patternRoot change");
    }

};