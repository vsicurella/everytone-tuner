/*
  ==============================================================================

    TuningFileParser.cpp
    Created: 23 Jul 2020 8:04:50pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TuningFileParser.h"

TuningFileParser::TuningFileParser(juce::String filepath)
{
	loadFile(filepath);
}

TuningFileParser::TuningFileParser(juce::File file)
{
	readFile(file);
}

TuningFileParser::~TuningFileParser()
{
	tuningLoaded = nullptr;
}

void TuningFileParser::loadFile(juce::String absoluteFilePath)
{
	auto file = juce::File(absoluteFilePath);
	readFile(file);
}

void TuningFileParser::readFile(juce::File file)
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

int TuningFileParser::determineTuningType(juce::File tuningFileIn)
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

Tuning::CentsDefinition TuningFileParser::parseScalaFileDefinition(juce::File scalaFile)
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

	auto name = juce::String(sclImport.GetTuningName());
	if (juce::File::isAbsolutePath(name))
	{
		auto temp = juce::File(name);
		name = temp.getFileNameWithoutExtension();
	}
	auto description = juce::String(sclImport.GetScaleDescription());

	Tuning::CentsDefinition centsDefinition(centsTable);
	centsDefinition = Tuning::Definition 
	{
		reference,
		0,
		name,
		description		
	};

	return centsDefinition;
}

Tuning::CentsDefinition TuningFileParser::parseTunFileDefinition(juce::File tunFile)
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

		auto name = juce::String(tunSingleScale.GetKeys()[TUN::CSingleScale::KEY_Name]);
		if (juce::File::isAbsolutePath(name))
		{
			auto temp = juce::File(name);
			name = temp.getFileNameWithoutExtension();
		}
		auto description = juce::String(tunSingleScale.GetKeys()[TUN::CSingleScale::KEY_Description]);

		definition = Tuning::Definition
		{
			reference,
			0,
			name,
			description
		};

		return definition;
	}

	return Tuning::CentsDefinition();
}

void TuningFileParser::parseTuning(juce::File fileLoaded)
{
	auto tuningDefinition = Tuning::CentsDefinition();
	switch (type)
	{
	case TuningType::SCL:
		tuningDefinition = parseScalaFileDefinition(fileLoaded);
		break;

	case TuningType::TUN:
		tuningDefinition = parseTunFileDefinition(fileLoaded);
		break;
	}

	tuningLoaded = std::make_unique<Tuning>(tuningDefinition);
}