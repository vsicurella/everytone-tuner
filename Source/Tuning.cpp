/*
  ==============================================================================

    Tuning.cpp
    Created: 26 Nov 2019 9:16:30pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "Tuning.h"

Tuning::Tuning(IntervalDefinition definition)
    : tuningSize(definition.intervalCents.size()),
      rootMidiNote(definition.reference.rootMidiNote),
      rootMidiChannelIndex(definition.reference.rootMidiChannel - 1),
      rootFrequency(definition.reference.rootFrequency),
      transpose(definition.transpose),
      name(definition.name),
      description(definition.description)
{
	setupTuning(definition.intervalCents);
}

Tuning::Tuning(const Tuning& tuning)
    : tuningSize(tuning.tuningSize),
      rootMidiNote(tuning.rootMidiNote),
      rootMidiChannelIndex(tuning.rootMidiChannelIndex),
      rootFrequency(tuning.rootFrequency),
      periodCents(tuning.periodCents),
      transpose(tuning.transpose),
      name(tuning.name),
      description(tuning.description)
{
    setupTuning(tuning.getIntervalCentsTable());
}

void Tuning::setupTuning(const juce::Array<double>& cents)
{
    periodCents = cents.getLast();
    periodRatio = centsToRatio(periodCents);
    periodMts = periodCents / 100.0;

    auto tuningShifted = juce::Array<double>(cents);
    tuningShifted.remove(tuningShifted.getLast());
    tuningShifted.insert(0, 0);    

    Keytographer::Map<double>::Definition definition =
    {
        tuningSize,
        tuningShifted.data(),
        periodCents,
        0, /* pattern root */
        rootMidiNote, /* tuning index root */
    };

    tuningMap.reset(new Keytographer::Map<double>(definition));

    rebuildTables();
}

void Tuning::rebuildTables()
{
    tuningMap->setMapRoot(rootMidiIndex());
    tuningMap->setTranspose(transpose);
    
    const int root = rootMidiIndex();
    // Build ratio table
    int i = root;
    int end = i + tuningSize;
    double firstCents = tuningMap->at(i);
    double cents, ratio;
    while (i <= end)
    {
        cents = tuningMap->at(i) - firstCents;
        ratio = centsToRatio(cents);
        ratioTable.set(i, ratio);
        i++;
    }

    double periodRatio = centsToRatio(periodCents);
    rootMts = roundN(10, frequencyToMTS(rootFrequency));
    
    // Build Frequency and MTS note tables
    int offset, periods, degree;
    double intervalRatio, frequency, size = tuningSize;
    for (int t = 0; t < TUNING_TABLE_SIZE; t++)
    {
        offset = t - root;
        degree = modulo(offset, tuningSize);
        intervalRatio = ratioTable[degree];
        periods = floor((double)offset / size);
        frequency = pow(periodRatio, periods) * intervalRatio * rootFrequency;
        
        frequencyTable.set(t, frequency);
        double mts = roundN(10, frequencyToMTS(frequency));
        mtsTable.set(t, mts);
    }
}

void Tuning::setName(juce::String nameIn)
{
    name = nameIn;
}

void Tuning::setDescription(juce::String descIn)
{
	description = descIn;
}

void Tuning::setRootFrequency(double frequency)
{
    // TODO check mts limits?
    rootFrequency = frequency;
    rebuildTables();
}

void Tuning::setRootMidiNote(int midiNote)
{
    // check midi limits?
    rootMidiNote = midiNote;
    rebuildTables();
}

void Tuning::setRootMidiChannel(int midiChannel)
{
    rootMidiChannelIndex = modulo(midiChannel - 1, 16);
    rebuildTables();
}

void Tuning::setReference(Reference reference)
{
    rootMidiNote = reference.rootMidiNote;
    rootMidiChannelIndex = reference.rootMidiChannel - 1;
    rootFrequency = reference.rootFrequency;
    rebuildTables();
}

double Tuning::getPeriodCents() const
{
	return periodCents;
}

double Tuning::getPeriodSemitones() const
{
	return periodCents * 0.01;
}

double Tuning::getNoteInCents(int noteNumber, int channel) const
{
	return tuningMap->at(midiIndex(noteNumber, channel - 1));
}

double Tuning::getNoteInSemitones(int noteNumber, int channel) const
{
    return getNoteInCents(noteNumber, channel - 1) * 0.01;
}

double Tuning::getNoteFrequency(int noteNumber, int channel) const
{
    return frequencyTable[midiIndex(noteNumber, channel)];
}

double Tuning::frequencyTableAt(int tableIndex) const
{
    return frequencyTable[modulo(tableIndex, TUNING_TABLE_SIZE)];
}

double Tuning::getNoteInMTS(int noteNumber, int channel) const
{
   return mtsTable[midiIndex(noteNumber, channel - 1)];
}

double Tuning::mtsTableAt(int tableIndex) const
{
    return mtsTable[modulo(tableIndex, TUNING_TABLE_SIZE)];
}

Tuning::Reference Tuning::getReference() const
{
    Reference r =
    {
        rootMidiNote,
        rootMidiChannelIndex + 1,
        rootFrequency
    };

    return r;
}

Tuning::Definition Tuning::getDefinition() const
{
    Definition d =
    {
        getReference(),
        transpose,
        name,
        description
    };

    return d;
}

juce::Array<double> Tuning::getIntervalCentsTable() const
{
    juce::Array<double> cents;
    int index;
    for (int i = 0; i <= tuningSize; i++)
    {
        index = i + rootMidiIndex();
        cents.set(i, tuningMap->at(index));
    }

    return cents;
}
juce::Array<double> Tuning::getIntervalRatioTable() const
{
    return ratioTable;
}
juce::Array<double> Tuning::getFrequencyTable() const
{
    return frequencyTable;
}
juce::Array<double> Tuning::getMTSTable() const
{
    return mtsTable;
}

juce::Array<MTSTriplet> Tuning::getMTSDataTable() const
{
    juce::Array<MTSTriplet> table;
    for (int i = 0; i < TUNING_TABLE_SIZE; i++)
        table.set(i, mtsNoteToTriplet(mtsTable[i]));
    return table;
}

template <typename U>
juce::String Tuning::tableToString(const juce::Array<U>& table, int startMidiChannel, int endMidiChannel) const
{
    juce::String tableStr = "";
    for (int ch = startMidiChannel - 1; ch < endMidiChannel - 1; modulo(ch + 1, 16))
    {
        int offset = midiIndex(0, ch);
        tableStr << "Channel " << juce::String(ch + 1) << ":" << juce::newLine;

        for (int i = 0; i < 128; i++)
            tableStr << juce::String(table[offset + i]) << juce::newLine;
    }
    return tableStr;
}

juce::String Tuning::mtsTableToString(int startMidiChannel, int endMidiChannel) const
{
    return tableToString<double>(mtsTable, startMidiChannel, endMidiChannel);
}

int Tuning::getScaleDegree(int noteNumber) const
{
	return tuningMap->patternAt(noteNumber);
}

int Tuning::closestNoteIndex(double mts) const
{
    int root = rootMidiIndex();

    if (tuningSize == 1)
    {
        return root + (int)round((mts - rootMts) / (periodCents * 0.01));
    }

    //mts = roundN(6, mts);

    //// Reduce into the first period
    //auto reduced = modulo(mts, periodMts);
    //
    //// Find index it's closest to
    //int closestDegree = 0;
    //int root = rootMidiIndex();
    //double difference = periodCents;
    //for (int i = 0; i <= tuningSize; i++)
    //{
    //    auto note = tuningMap->at(root + i) * 0.01;
    //    auto dif = abs(note - reduced);
    //    if (dif < difference)
    //    {
    //        difference = dif;
    //        closestDegree = i;
    //    }

    //    if (difference == 0)
    //        break;
    //}

    //int rootPeriods = floor((mts - rootMts) / 12.0);
    //int rootOffset = rootPeriods * tuningSize + closestDegree;
    //    /*if (rootPeriods < 0)
    //        rootOffset += closestDegree;
    //    else
    //        rootOffset += (tuningSize - closestDegree);*/

    
    // This assumes the scale pattern doesn't have intervals that jump beyond the period

    int rootPeriods = floor((mts - rootMts) / 12.0);
    int rootOffset = rootPeriods * tuningSize;

    int closestIndex = root + rootOffset;
    double difference = periodCents;
    for (int i = 0; i <= tuningSize; i++)
    {
        int index = root + rootOffset + i;
        auto note = mtsTableAt(index);
        auto dif = abs(note - mts);
        if (dif < difference)
        {
            difference = dif;
            closestIndex = index;
        }

        if (difference == 0)
            break;
    }

    return  closestIndex;
}

int Tuning::closestMtsNote(double mtsIn) const
{
    int index = closestNoteIndex(mtsIn);
    index = (index < 0) ? 0 : (index > 2047) ? 2047 : index;
    return mtsTable[index];
}