/*
  ==============================================================================

    Map.h
    Created: 11 Nov 2021 10:50:44pm
    Author:  Vincenzo

    Repeat mapping patterns indefinitely

  ==============================================================================
*/

#pragma once

#include <memory>
#include <cmath>
#include <functional>
#include <vector>
#include <string>


static int mod(int num, int mod)
{
    return ((num % mod) + mod) % mod;
}

template <typename T>
class Map
{
public:
    using Pattern = std::vector<T>;
    using PatternFunction = std::function<T(int x)>;

    struct Definition
    {
        int mapSize = 0;
        const T* pattern = nullptr;
        T patternBase = 0;
        int patternRootIndex = 0;
        int mapRootIndex = 0;
        T transpose = 0;

        bool operator==(const Definition& def)
        {
            bool equal = mapSize == def.mapSize;
            if (!equal) return false;

            for (int i = 0; i < mapSize; i++)
                if (pattern[i] != def.pattern[i])
                    return false;
            
            return patternBase == def.patternBase
                && patternRootIndex == def.patternRootIndex
                && mapRootIndex == def.mapRootIndex
                && transpose == def.transpose;
        }

        //std::string toString() const
        //{
        //    std::string size = 
        //    std::string str = "Size: " + mapSize
        //           //<< "\nPattern   : " << arrayToString<T>(def.pattern, def.mapSize, false);
        //           << "\nBase      : " + patternBase
        //           << "\nPat. Root : " + patternRootIndex
        //           << "\nMap Root  : " + mapRootIndex
        //           << "\nTranspose : " + transpose;
        //    return str;
        //}
    };

    struct FunctionDefinition
    {
        int mapSize = 0;            // Iterations of the function
        int start = 0;              // Starting value
        PatternFunction function;   // Integer input, T output
        T base = 0;                 // Number to add each cycle
        T transpose = 0;            // Number to add to function output
    };

private:

    // Size of map pattern
    int mapSize;

    // Integer mapping pattern
    Pattern mapPattern;
        
    // Modulo base for map pattern repetition
    T patternBase = 0;
        
    // Pattern index to consider the root
    int patternRootIndex = 0;
        
    // Index to place the root on
    int mapRootIndex = 0;

    // Added to mapped number
    T transpose = 0;

private:
        
    int normalizedIndex(const int& index) const { return index - mapRootIndex + (int)patternRootIndex; }
        
    int mapIndex(const int& normalizedIndex) const { return mod(normalizedIndex, mapSize); }

    int periods(const int& normalizedIndex) const { return floor((double)normalizedIndex / (double)mapSize); }

public:

    void setPatternRoot(int index) { patternRootIndex = mod(index, mapSize); }

private:

    static Pattern newPatternCopy(T size, const T* source)
    {
        Pattern pattern;
        for (int i = 0; i < size; i++)
            pattern.push_back(source[i]);

        return pattern;
    }

    Pattern initializePattern(Definition definition)
    {
        return newPatternCopy(definition.mapSize, definition.pattern);
    }

    Pattern initializePattern(FunctionDefinition definition)
    {
        Pattern pattern;
        for (int i = 0; i < definition.mapSize; i++)
        {
            int x = definition.start + i;
            pattern.push_back(definition.function(x));
        }

        return pattern;
    }

public:

    // 1:1 mapping
    Map()
        : mapSize(1),
          mapPattern({ 1 }),
          patternBase(1),
          patternRootIndex(0),
          mapRootIndex(0),
          transpose(0) {}

    Map(Definition definition)
        : mapSize(definition.mapSize), 
          mapPattern(initializePattern(definition)),
          patternBase(definition.patternBase),
          patternRootIndex(mod(definition.patternRootIndex, definition.mapSize)),
          mapRootIndex(definition.mapRootIndex),
          transpose(definition.transpose) {}

    Map(FunctionDefinition definition)
        : mapSize(definition.mapSize), 
          mapPattern(initializePattern(definition)),
          patternBase(definition.base),
          patternRootIndex(0),
          mapRootIndex(0),
          transpose(definition.transpose) {}

    Map(const Map& map)
        : mapSize(map.mapSize),
          mapPattern(initializePattern(map.definition())),
          patternBase(map.patternBase),
          patternRootIndex(map.patternRootIndex),
          mapRootIndex(map.mapRootIndex),
          transpose(map.transpose) {}

    ~Map() {}

    void operator=(const Map& map)
    {
        mapSize = map.mapSize;
        mapPattern = initializePattern(map.definition());
        patternBase = map.patternBase;
        patternRootIndex  = map.patternRootIndex;
        mapRootIndex = map.mapRootIndex;
        transpose = map.transpose;
    }

    Definition definition() const
    {
        Definition d =
        {
            mapSize,
            mapPattern.data(),
            patternBase,
            patternRootIndex,
            mapRootIndex,
            transpose
        };

        return d;
    }

    int size() const { return mapSize; }

    T base() const { return patternBase; }

    Pattern pattern() const { return mapPattern; }

    int patternRoot() const { return patternRootIndex; }

    int mapRoot() const { return mapRootIndex; }

    T transposition() const { return transpose; }

    T at(int index) const
    {
        auto normIndex = normalizedIndex(index);
        auto p = periods(normIndex);
        auto i = mapIndex(normIndex);
        return mapPattern[i] + p * patternBase + transpose;
    }

    int periodsAt(int index) const
    {
        auto normIndex = normalizedIndex(index);
        return periods(normIndex);
    }

    int mapIndexAt(int index) const
    {
        auto normIndex = normalizedIndex(index);
        return mapIndex(normIndex);
    }

    T patternAt(int index) const
    {
        return mapPattern[mapIndexAt(index)];
    }

    void setBase(int baseIn) { patternBase = baseIn; }

    void setMapRoot(int rootIndexIn) { mapRootIndex = rootIndexIn; }

    void setTranspose(int transposeIn) { transpose = transposeIn; }

    int closestIndexTo(T value)
    {
        int valuePeriod = (patternBase == 0)
            ? 0
            : floor(value / patternBase);

        T baseOffset = valuePeriod * patternBase;
        int indexOffset = normalizedIndex(valuePeriod * mapSize);

        T discrepancy = 10e12, difference = 0;
        int closestIndex = 0;
        for (int i = 0; i <= mapSize; i++)
        {
            T mapValue = (i < mapSize)
                ? baseOffset + mapPattern[i]
                : baseOffset + patternBase + mapPattern[0];

            difference = abs(value - mapValue);

            // TODO Might want to handle this differently...
            auto rDifference = round(difference * 10e8);
            auto rDiscrepancy = round(discrepancy * 10e8);

            if (rDifference < rDiscrepancy)
            {
                discrepancy = difference;
                closestIndex = indexOffset + i;
            }
        }

        return closestIndex;
    }
};

template <typename T>
static Map<T> LinearMap(int mapRootIndex = 0, T base = 1, T transpose = 0)
{
    Map<T>::FunctionDefinition d = 
    { 
        1,             /* size */
        0,             /* start */
        [=](int x) -> int { return x - mapRootIndex; }, 
        base,
        transpose 
    };
    auto map = Map<T>(d);
    return map;
}
