/*
  ==============================================================================

    TestsCommon.h
    Created: 14 Dec 2021 12:55:23pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#define E_ROUND 1e+8
#define TABLE_EPSILON 1e-5
#define MAX_FREQ 12543.854

static int precision(double number)
{
    auto p = log(abs(number));
    if (p < 6)
        return 6;
    return -p + 12;
}

class MultimapperUnitTest : public juce::UnitTest
{
protected:
    juce::String name;

public:

    MultimapperUnitTest(juce::String className) : name(className), juce::UnitTest(className) {}

    virtual ~MultimapperUnitTest() override {}

    virtual void runTest() = 0;

protected:

    template <typename T>
    juce::String testErrorMessage(juce::String keyName, T expected, T actual)
    {
        auto e = juce::var(expected);
        auto a = juce::var(actual);
        juce::String msg = name + "." + keyName + " should equal " + e.toString() + " but is " + a.toString();
        return msg;
    }

    template <typename T>
    void expect_exact(T expected, T actual, juce::String keyName)
    {
        expect(expected == actual, testErrorMessage(keyName, expected, actual));
    }

    template <typename T>
    void expect_equals(T expected, T actual, juce::String keyName)
    {
        auto dif = abs(actual - expected);
        expect(dif < TABLE_EPSILON, testErrorMessage(keyName, expected, actual));
    }

    template <typename T>
    void expect_pointers(T* expected, T* actual, juce::String keyName)
    {
        //auto e = std::reinterpret_pointer_cast<const char*>(&expected);
        //auto a = std::reinterpret_pointer_cast<const char*>(&actual);
        //auto eStr = juce::String(e);
        //auto aStr = juce::String(a);
        auto eStr = juce::String::createStringFromData(&expected, sizeof(&expected));
        auto aStr = juce::String::createStringFromData(&actual, sizeof(&actual));
        expect(expected == actual, testErrorMessage(keyName, eStr, aStr));
    }

    template <typename T>
    void test_getter(T expected, juce::String name, std::function<T(void)> getFunction)
    {
        auto value = getFunction();
        expect(expected == value, testErrorMessage(name, expected, value));
    }

    void test_table(int expectedSize, const double* expected, const juce::Array<double>& table, juce::String tableName)
    {
        for (int i = 0; i < expectedSize; i++)
        {
            auto ex = expected[i];
            auto p = precision(expected[i]);
            auto roundProduct = pow(10, p);
            auto roundQuotient = 1.0 / roundProduct;
            auto epsilon = pow(10, (p >= 6) ? -5 : -p + 2);

            auto e = round(expected[i] * roundProduct) * roundQuotient;
            auto t = round(table[i] * roundProduct) * roundQuotient;
            auto dif = abs(t - e);

            expect(dif <= epsilon, testErrorMessage(tableName + " at " + juce::String(i), e, t));
        }
    }
};