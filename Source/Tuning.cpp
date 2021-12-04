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
      periodCents(definition.intervalCents.getLast()),
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
        mtsTable.set(t, frequencyToMTS(frequency));
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

double Tuning::getNoteInMTS(int noteNumber, int channel) const
{
   return mtsTable[midiIndex(noteNumber, channel - 1)];
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

int Tuning::closestNoteToCents(double cents) const
{
    if (tuningSize == 1)
    {
        return round(cents / periodCents) + rootMidiNote;
    }

    // Reduce into the first period
    auto reduced = modulo(cents, periodCents);
    
    // Find degree it's closest to
    int closestDegree = 0;
    double difference = periodCents;
    for (int i = 0; i < tuningSize; i++)
    {
        auto dif = abs(tuningMap->patternAt(i) - cents);
        if (dif < difference)
        {
            difference = dif;
            closestDegree = i;
        }
    }

    int closestNote = closestDegree + floor(cents / periodCents) * tuningSize + rootMidiNote;
    return  closestNote;
}