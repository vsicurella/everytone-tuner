/*
  ==============================================================================

    TuningMath.h
    Created: 9 Jun 2020 1:26:25pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

static int modulo(int num, int mod)
{
    return ((num % mod) + mod) % mod; 
}

static double modulo(double num, double mod)
{
    return num - (floor(num / mod) * mod);
}

enum IntervalType
{
	Invalid = 0,
	Cents = 1,
	Ratio = 2,
	Decimal = 3,
	NofEDO = 4
};

static IntervalType getIntervalType(juce::String lineIn)
{
	bool hasDecimal = false;
	bool hasFraction = false;
	bool hasComma = false;
	bool hasBackslash = false;

	auto hasAType = [&hasDecimal, &hasFraction, &hasComma, &hasBackslash]()
	{
		return hasDecimal || hasFraction || hasComma || hasBackslash;
	};

	for (int i = 0; i < lineIn.length(); i++)
	{
		char c = lineIn[i];

		if (c == 92)
		{
			if (hasAType())
				return IntervalType::Invalid;

			hasBackslash = true;
		}

		else if (c < 44 || c > 59 || c == 45)
			return IntervalType::Invalid;

		else if (c == 46)
		{
			if (hasAType())
				return IntervalType::Invalid;

			hasDecimal = true;
		}

		else if (c == 47 || c == 58)
		{
			if (hasAType())
				return IntervalType::Invalid;

			hasFraction = true;
		}


		else if (c == 44)
		{
			if (hasAType())
				return IntervalType::Invalid;

			hasComma = true;
		}
	}

	if (hasDecimal)
		return IntervalType::Cents;

	else if (hasFraction)
		return IntervalType::Ratio;

	else if (hasComma)
		return IntervalType::Decimal;

	else if (hasBackslash)
		return IntervalType::NofEDO;

	return IntervalType::Invalid;
}

static double centsToRatio(double centsIn)
{
	return pow(2, centsIn / 1200);
}

static double ratioToCents(double ratioIn)
{
	return log2(ratioIn) * 1200;
}

static double ratioToSemitones(double ratioIn)
{
	return log2(ratioIn) * 12.0;
}

static double mtsToFrequency(double mts)
{
	return pow(2, (mts - 69) / 12.0) * 440.0;
}

static double mtsDataToFrequency(double mtsData)
{

}

static double frequencyToMTS(double freqIn)
{
	return 69 + 12 * log2(freqIn / 440.0);
}

//// Returns the 16-bit 
//static frequencyToMTSDifference(double freqIn)
//{
//	auto mts = frequencyToMTS(freqIn);
//	if (mts < 0)
//		return 0;
//
//	auto stdNote = trunc(mts);
//	juce::uint8 byte1 = (juce::uint8)stdNote;
//
//	// this can be improved
//	auto stdFreq = mtsToFrequency(stdNote);
//
//	double remainder = freqIn - stdFreq;
//	juce::uint8 byte2 = trunc(remainder / )
//	
//}

static double parseRatio(juce::String ratioIn)
{
	juce::StringRef separator = ratioIn.containsChar(':') ? ":" : "/";
	return ratioIn.upToFirstOccurrenceOf(separator, false, true).getDoubleValue() 
		/ ratioIn.fromLastOccurrenceOf(separator, false, true).getDoubleValue();
}

static double convertInterval(IntervalType typeIn, IntervalType typeOut, juce::String intervalIn)
{
	if (typeIn == IntervalType::Cents && typeOut == IntervalType::Ratio)
	{
		return centsToRatio(intervalIn.getDoubleValue());
	}

	else if (typeIn == IntervalType::Ratio && typeOut == IntervalType::Cents)
	{
		return ratioToCents(parseRatio(intervalIn));
	}

	else if (typeIn == typeOut)
	{
		switch (typeIn)
		{
		case (IntervalType::Ratio):
			return parseRatio(intervalIn);
		default:
			return intervalIn.getDoubleValue();
		}
	}

	return -1.0;
}

static double intervalTo(IntervalType typeOut, juce::String intervalIn)
{
	return convertInterval(getIntervalType(intervalIn), typeOut, intervalIn);
}

/*
	Generates a tuning table in cents with a regular temperament algorithm.
	The first generator is used as the period will always use an amount of 1 with no offset, regardless of provided values.
*/
static juce::Array<double> generateRegularTemperament(juce::Array<double> generatorCents,    juce::Array<int> generatorAmounts,    juce::Array<int> generatorsDown)
{
    juce::Array<double> intervals;
	double period = generatorCents[0];

    juce::Array<double> genCents = { period };

	// Initialize generator values and verify sizes
	for (int g = 1; g < generatorCents.size(); g++)
	{
		genCents.add(generatorCents[g] * -generatorsDown[g]);

		double factor = period / generatorCents[g];

		if (factor == (int)factor)
			generatorAmounts.set(g, factor);
	}

	generatorAmounts.set(0, 1);
	
	for (int g = 0; g < generatorCents.size(); g++)
	{
		double cents = genCents[g];

		for (int i = 0; i < generatorAmounts[g]; i++)
		{
			if (cents < 0 || (g > 0 && cents >= period))
			{
				cents -= period * floor(cents / period);
			}

			intervals.add(cents);
			cents += generatorCents[g];
		}
	}

	intervals.sort();

	return intervals;
}