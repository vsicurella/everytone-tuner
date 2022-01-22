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

    auto ok = sclImport.ReadSCL(path.c_str());

    if (!ok)
    {
        showError(path, sclImport.Err());
        return nullptr;
    }
    
    TUN::CSingleScale tunSingleScale;
    sclImport.SetSingleScale(tunSingleScale);

    ok = sclImport.Err().IsOK();
    if (!ok)
    {
        showError(path, tunSingleScale.Err());
        return nullptr;
    }

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

    auto vSize = (double)centsTable.size();
    CentsDefinition definition =
    {
        centsTable,
        baseFreq,
        name,
        description,
        centsTable.getLast(),
        vSize
    };

    return std::make_shared<FunctionalTuning>(definition);
}

std::shared_ptr<TuningTable> TuningFileParser::parseTunFileDefinition(juce::File tunFile)
{
    TUN::CSingleScale tunSingleScale;

    std::string path = tunFile.getFullPathName().toStdString();

    auto code = tunSingleScale.Read(path.c_str());
    if (code < 1)
    {
        showError(path, tunSingleScale.Err());
        return nullptr;
    }

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

    // Try to find interval pattern to return FunctionalTuning if possible
    auto extractedDefinition = CentsDefinition::ExtractFromFrequencyTable(frequencies, baseNote);
    if (extractedDefinition.intervalCents.size() > 0)
    {
        extractedDefinition.name = name;
        extractedDefinition.description = description;
        return std::make_shared<FunctionalTuning>(extractedDefinition);
    }

    // Fallback to a frequency table tuning
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

void TuningFileParser::showError(juce::String fileName, TUN::CErr err)
{
    juce::AlertWindow::showMessageBoxAsync(
        juce::MessageBoxIconType::WarningIcon, 
        juce::String("Error loading ") + fileName,
        juce::String(err.GetLastError()), 
        juce::String("OK"));
}
