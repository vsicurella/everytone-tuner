/*
  ==============================================================================

    CentsDefinition.h
    Created: 24 Dec 2021 10:11:23pm
    Author:  Vincenzo

    Tuning definition based on a list of cents values

    TODO remove JUCE dependency

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "TuningTable.h"

#include "../tests/TestsCommon.h"

struct CentsDefinition
{
    juce::Array<double> intervalCents = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200 };

    double rootFrequency = 440.0;

    juce::String name;
    juce::String description;

    double virtualPeriod = 0;
    double virtualSize = 0;

    static void getEqualDivisionsNameAndDescription(juce::String& name, juce::String& description, double divisions, double period, bool isCents = false)
    {
        bool integerDivisions = (int)divisions == divisions;
        auto strDivisions = (integerDivisions)
            ? juce::String((int)divisions)
            : juce::String(divisions);

        name = strDivisions + "-ed";

        if ((period == 1200.0 && isCents) || (period == 2.0 && !isCents))
        {
            name += "o";
            description = "The octave";
        }
        else
        {
            if (isCents)
            {
                description = juce::String(period) + " cents";
                name += description;
            }
            else
            {
                bool integerPeriod = (int)period == period;
                auto strPeriod = (integerPeriod)
                    ? juce::String((int)period)
                    : juce::String(period);

                name += strPeriod;
                description = strPeriod;
            }
        }

        description += " divided into " + strDivisions + " equal steps.";
    }

    bool operator==(const CentsDefinition& d) const
    {
        return intervalCents == d.intervalCents
            && rootFrequency == d.rootFrequency
            && name == d.name
            && description == d.description;
    }
    bool operator!=(const CentsDefinition& d) const { return !operator==(d); }

    juce::String getPeriodString() const { return juce::String(virtualPeriod) + " cents"; }

    juce::String getSizeString() const { return juce::String(virtualSize); }

    static void calculateMtsRootAndTableSize(const juce::Array<double>& intervalCents, double rootFrequency, int& rootIndex, int& tableSize)
    {
        double minCents = 10e10, maxCents = -10e10;
        double minIndex = -1, maxIndex = -1;

        for (int i = 0; i < intervalCents.size(); i++)
        {
            auto cents = intervalCents[i];
            if (cents < minCents)
            {
                minCents = cents;
                minIndex = i;
            }
            else if (cents > maxCents)
            {
                maxCents = cents;
                maxIndex = i;
            }
        }

        double lowestRatio = MTS_LOWEST_FREQ / rootFrequency;
        double lowestCents = ratioToCents(lowestRatio);

        double highestRatio = MTS_HIGHEST_FREQ / rootFrequency;
        double highestCents = ratioToCents(highestRatio);

        // Ceil & Floor are preferred for MTS range,
        // but tests are currently set up for rounding.
        // Minor problem, generally just affects things by 1 index
        if (intervalCents.size() == 1)
        {
            int lowest = /*ceil*/round(lowestCents / intervalCents[0]);
            int highest = /*floor*/round(highestCents / intervalCents[0]);

            rootIndex = -lowest;
            tableSize = highest - lowest + 1;
            return;
        }

        auto periodCents = intervalCents.getLast();
        auto minPeriod = (minCents < 0) ? periodCents - minCents : periodCents;
        double maxPeriod = (maxCents > periodCents) ? maxCents : periodCents;

        int lowestFromRoot = /*ceil*/round(lowestCents / minPeriod * intervalCents.size());
        int highestFromRoot = /*floor*/round(highestCents / maxPeriod * intervalCents.size());

        rootIndex = -lowestFromRoot;
        tableSize = highestFromRoot - lowestFromRoot + 1;
    }

    void calculateMtsRootAndTableSize(int& rootIndex, int& tableSize) const
    {
        calculateMtsRootAndTableSize(intervalCents, rootFrequency, rootIndex, tableSize);
    }

    static CentsDefinition CentsDivisions(double divisions, double periodCents = 1200.0, double frequency = 440.0)
    {
        double step = periodCents / divisions;

        juce::String name, description;
        getEqualDivisionsNameAndDescription(name, description, divisions, periodCents, true);

        CentsDefinition definition = { { step }, frequency, name, description, periodCents, divisions };
        return definition;
    }

    static CentsDefinition RatioDivisions(double divisions, double periodRatio = 2.0, double frequency = 440.0)
    {
        double step = ratioToCents(periodRatio) / divisions;

        juce::String name, description;
        getEqualDivisionsNameAndDescription(name, description, divisions, periodRatio);

        CentsDefinition definition = { { step }, frequency, name, description, ratioToCents(periodRatio), divisions };
        return definition;
    }

    static CentsDefinition ExtractFromTuningTable(const TuningTableBase* tuningTable)
    {
        double period = tuningTable->getVirtualPeriod();
        if (period == 0)
            period = 1200.0;

        double size = tuningTable->getVirtualSize();
        if (size == 0)
            size = tuningTable->getTableSize();

        auto cents = juce::Array<double>();
        for (int i = 0; i < size; i++)
        {
            auto c = tuningTable->centsFromRoot(i);
            cents.add(c);
        }

        auto definition = CentsDefinition
        {
            cents,
            tuningTable->getRootFrequency(),
            tuningTable->getName(),
            tuningTable->getDescription(),
            period,
            size
        };

        return definition;
    }

    static CentsDefinition ExtractFromFrequencyTable(juce::Array<double> frequencyTable, int rootIndex)
    {
        int maxSize = frequencyTable.size() - rootIndex;

        if (maxSize <= 0)
            return CentsDefinition({});

        // If root is not a whole number frequency, try to find one with an integer MTS value
        double rootFrequency = roundN(6, frequencyTable[rootIndex]);
        if (rootFrequency != (int)rootFrequency)
        {
            for (int i = 0; i < frequencyTable.size(); i++)
            {
                auto freq = frequencyTable[i];
                if (freq == 0) // Corrupted/malformed frequency table
                    return CentsDefinition({});

                auto mts = roundN(3, frequencyToMTS(freq));
                auto intMts = (int)mts;
                if (mts != intMts)
                    continue;

                rootIndex = i;
                rootFrequency = freq;
                maxSize = frequencyTable.size() - rootIndex;
                break;
            }
        }

        // I suspect there's a better way to do this

        // Convert frequency table to the first derivative of it's cents representation
        // ie. each index is how many cents away from the previous frequency

        juce::Array<double> stepsInCents;
        for (int i = 1; i < maxSize; i++)
        {
            int index = i + rootIndex;
            auto ratio = frequencyTable[index] / frequencyTable[index - 1];
            auto cents = roundN(3, ratioToCents(ratio));
            stepsInCents.add(cents);
        }

        // Test chunks of the cents-steps pattern, with incrementing sizes,
        // and figure out at what size the pattern appears to repeat

        int successfulSize = 0;
        for (int size = 1; size < stepsInCents.size(); size++)
        {
            bool sizeFailed = false;

            // debug
            juce::String chunkString = arrayToString(stepsInCents.getRawDataPointer(), size, false);
            DBG("Testing chunk: " + chunkString);

            int testIndex = size;
            while (testIndex < stepsInCents.size())
            {
                int patternIndex = testIndex % size;
                
                double patternStep = stepsInCents[patternIndex];
                double testStep = stepsInCents[testIndex];
                if (patternStep != testStep)
                {
                    sizeFailed = true;
                    break;
                }

                testIndex++;
            }

            if (sizeFailed)
                continue;

            successfulSize = size;
            break;
        }

        if (successfulSize == 0)
            return CentsDefinition({}); // Couldn't find a repeating pattern - should this return the whole table?

        stepsInCents.resize(successfulSize);
        DBG("Step pattern result: " + arrayToString(stepsInCents.data(), stepsInCents.size()));

        juce::Array<double> centsScale(0.0);
        for (int i = 1; i < successfulSize + 1; i++)
        {
            auto interval = stepsInCents[i - 1] + centsScale[i - 1];
            centsScale.add(interval);
        }
        centsScale.remove(0);

        DBG("Scale:\n" + arrayToString(centsScale.data(), centsScale.size()));

        CentsDefinition definition =
        {
            centsScale,
            frequencyTable[rootIndex]
        };

        definition.virtualPeriod = centsScale.getLast();    
        definition.virtualSize = centsScale.size();

        return definition;
    }
};
