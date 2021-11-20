/*
  ==============================================================================

    Tuning.cpp
    Created: 26 Nov 2019 9:16:30pm
    Author:  Vincenzo Sicurella

  ==============================================================================
*/

#include "Tuning.h"

Tuning::Tuning(const juce::Array<double>& centsTable, int rootMidiNoteIn, juce::String nameIn, juce::String descriptionIn)
    : tuningSize(centsTable.size()),
      rootMidiNote(rootMidiNoteIn),
      periodCents(centsTable.getLast()),
      name(nameIn),
      description(descriptionIn)
{
	setupTuning(centsTable);
}

Tuning::Tuning(const Tuning& tuning)
    : tuningSize(tuning.tuningSize),
      rootMidiNote(tuning.rootMidiNote),
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

double Tuning::getNoteInSemitones(int noteNumber) const
{
    return tuningMap->at(noteNumber) * 0.01;
}

double Tuning::getNoteInCents(int noteNumber) const
{
	return tuningMap->at(noteNumber);
}

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