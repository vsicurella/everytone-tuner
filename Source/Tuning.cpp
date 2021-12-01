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
      rootMidiNote(definition.rootMidiNote),
      rootMidiChannel(definition.rootMidiChannel),
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

void Tuning::setRootNote(int rootNoteIn)
{
	rootMidiNote = rootNoteIn;
    tuningMap->setMapRoot(rootMidiNote);
    tuningMap->setTranspose(rootMidiNote);
}

void Tuning::setDescription(juce::String descIn)
{
	description = descIn;
}

double Tuning::getPeriodCents() const
{
	return periodCents;
}

double Tuning::getPeriodSemitones() const
{
	return periodCents / 100.0;
}

int Tuning::getTuningSize() const
{
	return tuningSize;
}

int Tuning::getRootNote() const
{
    return rootMidiNote;
}

double Tuning::getNoteInCents(int noteNumber, int tableIndex = 0) const
{
	return tuningMap->at(tableIndex * 128 + noteNumber);
}

double Tuning::getNoteInSemitones(int noteNumber, int tableIndex = 0) const
{
    return getNoteInCents(noteNumber, tableIndex) * 0.01;
}

//double Tuning::getNoteInMTS(int noteNumber, int tableIndex = 0) const
//{
//    auto cents = getNoteInCents(noteNumber, tableIndex);
//    auto 
//}

juce::String Tuning::getName() const
{
	return name;
}

juce::String Tuning::getDescription() const
{
	return description;
}

juce::String Tuning::toString() const
{
	juce::String tableStr = "";
    for (int i = 0; i < 128; i++)
		tableStr << juce::String(tuningMap->at(i)) << juce::newLine;

	return tableStr;
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