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
#include "TuningMath.h"

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


    // I think this should work alright when given a frequency table with an accurate root, but I'm having
    // trouble getting this using an unmodified TUN library, so this is shelved for the moment.
    //static CentsDefinition ExtractFromFrequencyTable(juce::Array<double> frequencyTable, int rootIndex)
    //{
    //    int maxSize = frequencyTable.size() - rootIndex;

    //    if (maxSize <= 0)
    //        return CentsDefinition({ 0 });


    //    // I suspect there might be a better way to do this

    //    juce::Array<double> centsSteps;
    //    for (int i = 1; i < maxSize; i++)
    //    {
    //        int index = i + rootIndex;
    //        auto ratio = frequencyTable[index] / frequencyTable[index - 1];
    //        auto cents = roundN(6, ratioToCents(ratio));
    //        centsSteps.add(cents);
    //    }

    //    // Test chunks of the cents steps pattern, with incrementing sizes
    //    // And figure out at what size the pattern repeats

    //    int successfulSize = 0;
    //    for (int size = 1; size < centsSteps.size(); size++)
    //    {
    //        bool sizeFailed = false;
    //        int numChunks = (centsSteps.size() / size);

    //        juce::String chunkString;
    //        for (int i = 0; i < size; i++)
    //            chunkString += juce::String(centsSteps[i]) + ", ";
    //        DBG("Testing chunk: " + chunkString);

    //        for (int chunk = 0; chunk < numChunks; chunk++)
    //        {
    //            int chunkOffset = (size + 1) * chunk;

    //            for (int index = 0; index < size; index++)
    //            {
    //                int testIndex = chunkOffset + index;
    //                if (testIndex > centsSteps.size())
    //                    break;

    //                double chunkPattern = centsSteps[index];
    //                double chunkTest = centsSteps[testIndex];
    //                if (chunkPattern != chunkTest)
    //                {
    //                    sizeFailed = true;
    //                    break;
    //                }
    //            }

    //            if (sizeFailed)
    //                break;
    //        }

    //        if (!sizeFailed)
    //        {
    //            successfulSize = size;
    //            break;
    //        }
    //    }

    //    if (successfulSize == 0)
    //        return CentsDefinition({ 0 });

    //    centsSteps.resize(successfulSize);
    //    DBG("Selected: " + arrayToString(centsSteps.data(), centsSteps.size()));

    //    juce::Array<double> centsScale(0);
    //    for (int i = 0; i < successfulSize; i++)
    //    {
    //        auto interval = centsSteps[i] + centsScale[i - 1];
    //        centsScale.add(interval);
    //    }
    //    centsScale.remove(0);

    //    DBG("Scale: " + arrayToString(centsScale.data(), centsScale.size()));

    //    CentsDefinition definition =
    //    {
    //        centsScale,
    //        frequencyTable[rootIndex]
    //    };

    //    definition.virtualPeriod = centsSteps.getLast();
    //    definition.virtualSize = successfulSize;

    //    return definition;
    //}
};
