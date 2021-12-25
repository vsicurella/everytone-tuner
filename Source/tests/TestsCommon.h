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

using namespace juce;

static int precision(double number)
{
    auto p = log(abs(number));
    if (p < 6)
        return 6;
    return -p + 12;
}

template <typename T>
static String arrayToString(T const* array, int size, bool newLines = true)
{
    String str = "";
    for (int i = 0; i < size; i++)
    {
        str += String(i) + ": " + String(array[i]);
        if (newLines)
            str += newLine;
        else
            str += ", ";
    }

    return str;
}

//template <typename T>
//static String MapDefToString(Map<T>::Definition def)
//{
//    String str = "Size: " + String(def.mapSize);
//    str += "\nPattern   : " + arrayToString<T>(def.pattern, def.mapSize, false);
//    str += "\nBase      : " + String(def.patternBase);
//    str += "\nPat. Root : " + String(def.patternRootIndex);
//    str += "\nMap Root  : " + String(def.mapRootIndex);
//    str += "\nTranspose : " + String(def.transpose);
//    return str;
//}

class EverytoneTunerUnitTest : public UnitTest
{
protected:
    String name;

public:

    EverytoneTunerUnitTest(String className) : name(className), UnitTest(className) {}

    virtual ~EverytoneTunerUnitTest() override {}

    virtual void runTest() = 0;

protected:

    template <typename T>
    String testErrorMessage(String keyName, T expected, T actual)
    {
        auto e = var(expected);
        auto a = var(actual);
        String msg = name + "." + keyName + " should equal " + e.toString() + " but is " + a.toString();
        return msg;
    }

    template <typename T>
    void expect_exact(T expected, T actual, String keyName)
    {
        expect(expected == actual, testErrorMessage(keyName, expected, actual));
    }

    template <typename T>
    void expect_equals(T expected, T actual, String keyName)
    {
        auto dif = abs(actual - expected);
        expect(dif < TABLE_EPSILON, testErrorMessage(keyName, expected, actual));
    }

    template <typename T>
    void expect_pointers(T* expected, T* actual, String keyName)
    {
        auto eStr = String::createStringFromData(&expected, sizeof(&expected));
        auto aStr = String::createStringFromData(&actual, sizeof(&actual));
        expect(expected == actual, testErrorMessage(keyName, eStr, aStr));
    }

    template <typename T>
    void test_getter(T expected, String name, std::function<T(void)> getFunction)
    {
        auto value = getFunction();
        expect(expected == value, testErrorMessage(name, expected, value));
    }

    void test_table(int expectedSize, const double* expected, const Array<double>& table, String tableName)
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

            expect(dif <= epsilon, testErrorMessage(tableName + " at " + String(i), e, t));
        }
    }

    template <typename T, typename TARR>
    void test_table(int expectedSize, const T* expected, const TARR& table, String tableName)
    {
        for (int i = 0; i < expectedSize; i++)
        {
            int e = expected[i];
            int t = table[i];
            expect_exact(e, t, tableName + " at " + String(i));
        }
    }

    template <typename T>
    void test_integer_array(int expectedSize, const int* expected, const T& table, String tableName)
    {
        for (int i = 0; i < expectedSize; i++)
        {
            int e = expected[i];
            int t = table[i];
            expect_exact(e, t, tableName + " at " + String(i));
        }
    }
};