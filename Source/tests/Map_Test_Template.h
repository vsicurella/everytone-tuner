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
class Map_Test : public EverytoneTunerUnitTest
{
private:

    void testMap(int size, int start, const T* expected, const Map<T>* map, String name)
    {
        int end = start + size;
        for (int i = start; i < end; i++)
        {
            T e = expected[i - start];
            expect_exact<T>(e, map->at(i), name + " map at " + String(i));
        }
    };

    String MapDefToString(typename Map<T>::Definition def)
    {
        String str = "Size: " + String(def.mapSize);
        str += "\nPattern   : " + vecToString<T>(def.pattern, def.mapSize, false);
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
        Map<T>* map;

        const int testTableSize;
        const int testTableStart;
        const T* expectedTable = nullptr;

        int altMapRoot;
        T altTranspose;
        int altTableStart;
        const T* altExpectedTable = nullptr;

        // Transpose is not reset from altTable test
        int altPatternRootIndex;
        int altPatternRootStart;
        const T* altPatternRootExpectedTable = nullptr;
    };

private:

    Parameters params;

public:

    Map_Test(String nameOfType, Parameters testParams) 
        : EverytoneTunerUnitTest("Map " + nameOfType),
          params(testParams) {}

    virtual ~Map_Test() override
    {
        delete params.map;

        //if (params.definition.pattern != nullptr)
        //{
        //    delete[] params.definition.pattern;
        //    params.definition.pattern = nullptr;
        //}

        if (params.expectedTable != nullptr)
        {
            delete[] params.expectedTable;
        }

        if (   params.altExpectedTable != nullptr
            && params.altExpectedTable != params.expectedTable )
        {
            delete[] params.altExpectedTable;
        }


        if (   params.altPatternRootExpectedTable != nullptr
            && params.altPatternRootExpectedTable != params.expectedTable 
            && params.altPatternRootExpectedTable != params.altExpectedTable)
        {
            delete[] params.altPatternRootExpectedTable;
        }
    }

    void runTest() override
    {
        auto definition = params.definition;
        auto map = params.map;

        beginTest("Getters");
        expect_exact<T>(definition.mapSize, map->size(), "mapSize");
        test_table<T, std::vector<T>>(definition.mapSize, definition.pattern, map->pattern(), "pattern");
        expect_exact<T>(definition.patternBase, map->base(), "base");

        auto expectedPatternRoot = mod(definition.patternRootIndex, definition.mapSize);
        expect_equals<T>(expectedPatternRoot, map->patternRoot(), "patternRoot");

        auto patternRootValue = map->patternAt(map->patternRoot());
        expect_equals<T>(definition.pattern[expectedPatternRoot], patternRootValue, "patternRoot value");
        expect_equals<T>(definition.mapRootIndex, map->mapRoot(), "mapRoot");

        auto mapRootValue = map->patternAt(map->patternRoot());
        expect_equals<T>(definition.pattern[expectedPatternRoot], patternRootValue, "mapRoot value");

        expect_equals<T>(definition.transpose, map->transposition(), "transpose");

        auto redefinition = map->definition();
        expect(definition == redefinition, "Map defs are not equal:\nOG:\n" + MapDefToString(definition) + "\n\nNew:\n" + MapDefToString(redefinition));

        // Main map test
        testMap(params.testTableSize, params.testTableStart, params.expectedTable, map, params.testName);


        beginTest("Setters");
        T newBase = 0;
        Array<T> base0Table;
        base0Table.resize(params.testTableSize);
        for (int i = 0; i < params.testTableSize; i++)
            base0Table.set(i, definition.pattern[i % definition.mapSize]);

        map->setBase(newBase);
        testMap(params.testTableSize, params.testTableStart, base0Table.data(), map, params.testName + "base change");

        // reset defaultBase
        map->setBase(definition.patternBase);
        testMap(params.testTableSize, params.testTableStart, params.expectedTable, map, params.testName + "base reset");

        T transposeTest = 3;
        Array<T> mapTransposed;
        mapTransposed.resize(params.testTableSize);
        for (int i = 0; i < params.testTableSize; i++)
            mapTransposed.set(i, params.expectedTable[i] + transposeTest);

        map->setTranspose(transposeTest);
        testMap(params.testTableSize, params.testTableStart, mapTransposed.data(), map, params.testName + "transposed");

        map->setMapRoot(params.altMapRoot);
        map->setTranspose(params.altTranspose);
        testMap(params.testTableSize, params.altTableStart, params.altExpectedTable, map, params.testName + "root & transpose change");

        map->setPatternRoot(params.altPatternRootIndex);
        testMap(params.testTableSize, params.altPatternRootStart, params.altPatternRootExpectedTable, map, params.testName + "patternRoot change");
    }

};
