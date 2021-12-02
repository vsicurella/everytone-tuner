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
      rootMidiChannel(definition.reference.rootMidiChannel),
      rootFrequency(definition.reference.rootFrequency),
      periodCents(definition.intervalCents.getLast()),
      name(definition.name),
      description(definition.description)
{
	setupTuning(definition.intervalCents);
}

Tuning::Tuning(const Tuning& tuning)
    : tuningSize(tuning.tuningSize),
      rootMidiNote(tuning.rootMidiNote),
      rootMidiChannel(tuning.rootMidiChannel),
      rootFrequency(tuning.rootFrequency),
      periodCents(tuning.periodCents),
      name(tuning.name),
      description(tuning.description)
{
    tuningMap.reset(new Keytographer::Map<double>(*tuning.tuningMap.get()));
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
        0,
        rootMidiNote,
        rootMidiNote
    };

    tuningMap.reset(new Keytographer::Map<double>(definition));
}

void Tuning::rebuildTable()
{
    int midiIndex = rootMidiChannel * 128 + rootMidiNote;
    tuningMap->setMapRoot(midiIndex);
    tuningMap->setTranspose(midiIndex);

    for (int i = 0; i < TUNING_TABLE_SIZE; i++)
    {
        centsTable.set(i, tuningMap->at(i));
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

void Tuning::setFrequency(double frequency)
{
    // TODO check mts limits?
    rootFrequency = frequency;
    rebuildTable();
}

void Tuning::setRootMidiNote(int midiNote)
{
    // check midi limits?
    rootMidiNote = midiNote;
    rebuildTable();
}

void Tuning::setRootMidiChannel(int midiChannel)
{
    // check midi limits?
    rootMidiChannel = midiChannel;
    rebuildTable();
}

void Tuning::setReference(Reference reference)
{
    rootMidiNote = reference.rootMidiNote;
    rootMidiChannel = reference.rootMidiChannel;
    rootFrequency = reference.rootFrequency;
    rebuildTable();
}

double Tuning::getPeriodCents() const
{
	return periodCents;
}

double Tuning::getPeriodSemitones() const
{
	return periodCents * 0.01;
}

double Tuning::getNoteInCents(int noteNumber, int tableIndex) const
{
	return centsTable[midiIndex(noteNumber, tableIndex)];
}

double Tuning::getNoteInSemitones(int noteNumber, int tableIndex) const
{
    return getNoteInCents(noteNumber, tableIndex) * 0.01;
}

//double Tuning::getNoteInMTS(int noteNumber, int tableIndex = 0) const
//{
//    auto cents = getNoteInCents(noteNumber, tableIndex);
//    auto 
//}

Tuning::Reference Tuning::getReference() const
{
    Reference r =
    {
        rootMidiNote,
        rootMidiChannel,
        rootFrequency
    };

    return r;
}

Tuning::Definition Tuning::getDefinition() const
{
    Definition d =
    {
        getReference(),
        name,
        description
    };

    return d;
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

juce::String Tuning::centsTableToString(int startMidiChannel, int endMidiChannel) const
{
    return tableToString<double>(centsTable, startMidiChannel, endMidiChannel);
}

juce::String Tuning::semitoneTableToString(int startMidiChannel, int endMidiChannel) const
{
    juce::Array<double> semitoneTable(TUNING_TABLE_SIZE);
    for (int i = 0; i < TUNING_TABLE_SIZE; i++)
        semitoneTable.set(i, centsTable[i] * 0.01);
    return tableToString<double>(semitoneTable, startMidiChannel, endMidiChannel);
}

juce::String Tuning::mtsTableToString(int startMidiChannel, int endMidiChannel) const
{
    juce::Array<double> semitoneTable(TUNING_TABLE_SIZE);
    for (int i = 0; i < TUNING_TABLE_SIZE; i++)
        semitoneTable.set(i, centsTable[i] * 0.01);
    return tableToString<double>(semitoneTable, startMidiChannel, endMidiChannel);
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