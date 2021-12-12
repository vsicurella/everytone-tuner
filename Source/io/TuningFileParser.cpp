/*
  ==============================================================================

    TuningFileParser.cpp
    Created: 23 Jul 2020 8:04:50pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TuningFileParser.h"

TuningFileParser::TuningFileParser(const juce::String& absoluteFilePath)
{
	loadFile(absoluteFilePath);
}

TuningFileParser::~TuningFileParser()
{
	tuningLoaded = nullptr;
}

void TuningFileParser::loadFile(const juce::String& absoluteFilePath)
{
	filePath = absoluteFilePath;
	readFile(juce::File(filePath));
}

void TuningFileParser::readFile(const juce::File& file)
{
	DBG("File Size: " + juce::String(file.getSize()));
	type = TuningType(determineTuningType(file));
	parseTuning(file);
}

Tuning::Definition TuningFileParser::getTuningDefinition() const
{
	return tuningLoaded->getDefinition();
}

const Tuning* TuningFileParser::getTuning() const
{
	return tuningLoaded.get();
}

TuningFileParser::TuningType TuningFileParser::getTuningType() const
{
	return type;
}

int TuningFileParser::determineTuningType(const juce::File& tuningFileIn)
{
	int type = 0;

	// Check file extension

	juce::String fileType = tuningFileIn.getFileExtension().toLowerCase();

	if (fileType == ".scl")
		type = TuningType::SCL;

	else if (fileType == ".tun")
		type = TuningType::TUN;

	return type;
}

Tuning::CentsDefinition TuningFileParser::parseScalaFileDefinition(const juce::File& scalaFile)
{
	std::string path = scalaFile.getFullPathName().toStdString();

	TUN::CSCL_Import sclImport;

	sclImport.ReadSCL(path.c_str());
	
	TUN::CSingleScale tunSingleScale;
	sclImport.SetSingleScale(tunSingleScale);

	int baseNote = tunSingleScale.GetBaseNote();
	double baseFreq = tunSingleScale.GetBaseFreqHz();

	juce::Array<double> centsTable;
	double periodCents = sclImport.GetLineInCents(sclImport.GetScaleSize() - 1);

	for (int i = 0; i <= sclImport.GetScaleSize(); i++)
	{
		centsTable.add(sclImport.GetLineInCents(i));
	}

	auto reference = Tuning::Reference{ 1, baseNote, baseFreq };

	Tuning::CentsDefinition centsDefinition(centsTable);
	centsDefinition = Tuning::Definition 
	{
		reference,
		0,
		sclImport.GetTuningName(),
		sclImport.GetScaleDescription()
	};

	return centsDefinition;
}

Tuning::CentsDefinition TuningFileParser::parseTunFileDefinition(const juce::File& tunFile)
{
	TUN::CSingleScale tunSingleScale;
	std::string path = tunFile.getFullPathName().toStdString();

	if (tunSingleScale.Read(path.c_str()))
	{
		juce::Array<double> centsTable;
		int baseNote = tunSingleScale.GetBaseNote();
		double baseFreq = tunSingleScale.GetBaseFreqHz();

		for (auto freq : tunSingleScale.GetNoteFrequenciesHz())
		{
			centsTable.add(ratioToCents(freq / baseFreq));
		}

		Tuning::Reference reference = { 1, baseNote, baseFreq };
		Tuning::CentsDefinition definition(centsTable);
		definition = Tuning::Definition
		{
			reference,
			0,
			tunSingleScale.GetKeys()[TUN::CSingleScale::KEY_Name],
			tunSingleScale.GetKeys()[TUN::CSingleScale::KEY_Description]
		};

		return definition;
	}

	return Tuning::CentsDefinition();
}

void TuningFileParser::parseTuning(const juce::File& fileLoaded)
{
	switch (type)
	{
	case TuningType::SCL:
		tuningLoaded.reset(new Tuning(parseScalaFileDefinition(fileLoaded)));
		break;

	case TuningType::TUN:
		tuningLoaded.reset(new Tuning(parseTunFileDefinition(fileLoaded)));
		break;
	}
}