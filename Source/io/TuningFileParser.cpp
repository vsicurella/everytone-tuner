/*
  ==============================================================================

    TuningFileParser.cpp
    Created: 23 Jul 2020 8:04:50pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "TuningFileParser.h"

TuningFileParser::TuningFileParser(juce::File file)
{
    readFile(file);
}

TuningFileParser::~TuningFileParser()
{
    parsedTuning = nullptr;
}

void TuningFileParser::readFile(juce::File file)
{
    type = TuningType(determineTuningType(file));
    parseTuning(file);
}

std::shared_ptr<TuningTable> TuningFileParser::getTuning() const
{
    return parsedTuning;
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

std::shared_ptr<FunctionalTuning> TuningFileParser::parseScalaFileDefinition(juce::File scalaFile)
{
    std::string path = scalaFile.getFullPathName().toStdString();

    TUN::CSCL_Import sclImport;

    sclImport.ReadSCL(path.c_str());
    
    TUN::CSingleScale tunSingleScale;
    sclImport.SetSingleScale(tunSingleScale);

    int baseNote = tunSingleScale.GetBaseNote();
    double baseFreq = tunSingleScale.GetBaseFreqHz();

    juce::Array<double> centsTable;
    for (int i = 1; i <= sclImport.GetScaleSize(); i++)
    {
        auto line = sclImport.GetLineInCents(i);
        centsTable.add(line);
    }

    auto name = juce::String(sclImport.GetTuningName());
    if (juce::File::isAbsolutePath(name))
    {
        auto temp = juce::File(name);
        name = temp.getFileNameWithoutExtension();
    }
    auto description = juce::String(sclImport.GetScaleDescription());

    CentsDefinition definition =
    {
        centsTable,
        baseFreq,
        name,
        description,
        centsTable.getLast(),
        centsTable.size(),
    };

    return std::make_shared<FunctionalTuning>(definition);
}

std::shared_ptr<TuningTable> TuningFileParser::parseTunFileDefinition(juce::File tunFile)
{
    TUN::CSingleScale tunSingleScale;

    std::string path = tunFile.getFullPathName().toStdString();

    if (tunSingleScale.Read(path.c_str()) < 1)
        return nullptr;

    juce::Array<double> centsTable;
    int baseNote = tunSingleScale.GetBaseNote();
    double baseFreq = tunSingleScale.GetBaseFreqHz();

    juce::Array<double> frequencies;
    for (auto freq : tunSingleScale.GetNoteFrequenciesHz())
        frequencies.add(freq);

    auto name = juce::String(tunSingleScale.m_strName);
    if (juce::File::isAbsolutePath(name))
    {
        auto temp = juce::File(name);
        name = temp.getFileNameWithoutExtension();
    }
    auto description = tunInfoToString(tunSingleScale);

    TuningTable::Definition definition =
    {
        frequencies,
        baseNote,
        name,
        description
    };

    return std::make_shared<TuningTable>(definition);
}

void TuningFileParser::parseTuning(juce::File fileLoaded)
{
    switch (type)
    {
    case TuningType::SCL:
        parsedTuning = parseScalaFileDefinition(fileLoaded);
        break;

    case TuningType::TUN:
        parsedTuning = parseTunFileDefinition(fileLoaded);
        break;
    }
}

juce::String TuningFileParser::tunInfoToString(const TUN::CSingleScale& tunScale)
{
    juce::String info;

    juce::StringArray keys =
    {
        "Name",
        "ID",
        "Filename",
        "Author",
        "Date",
        "Location",
        "Contact",
        "Editor",
        "EditorSpecs",
        "Description",
        "Keywords",
        "History",
        "Geography",
        "Instrument",
        "Compositions",
        "Comments"
    };

    juce::StringArray values;
    values.add(tunScale.m_strName);
    values.add(tunScale.m_strID);
    values.add(tunScale.m_strFilename);
    values.add(tunScale.m_strAuthor);
    values.add(tunScale.GetDate());
    values.add(tunScale.m_strLocation);
    values.add(tunScale.m_strContact);
    values.add(tunScale.m_strEditor);
    values.add(tunScale.m_strEditorSpecs);
    values.add(tunScale.m_strDescription);

    auto keywords = juce::StringArray();
    for (auto word : tunScale.m_lstrKeywords)
        keywords.add(word);
    values.add(keywords.joinIntoString(", "));
    
    values.add(tunScale.m_strHistory);
    values.add(tunScale.m_strGeography);
    values.add(tunScale.m_strInstrument);
    
    auto compositions = juce::StringArray();
    for (auto composition : tunScale.m_lstrCompositions)
        compositions.add(composition);
    values.add(compositions.joinIntoString(", "));
    
    values.add(tunScale.m_strComments);

    for (int i = 0; i < keys.size(); i++)
    {
        auto value = values[i];
        if (value.isEmpty())
            continue;

        auto str = keys[i] + ": " + value + juce::newLine;
        info += str + juce::newLine;
    }

    return info;
}