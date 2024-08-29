/*
  ==============================================================================

    WaveShaper.cpp
    Created: 17 Nov 2023 4:40:32pm
    Author:  kylew

  ==============================================================================
*/

#include "WaveShaper.h"

WaveShaperComp::WaveShaperComp(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    updateRSWL(apvts, tb);

    addAndMakeVisible(*select);
    addAndMakeVisible(*distort);
    addAndMakeVisible(*inGain);
    addAndMakeVisible(*mix);
    addAndMakeVisible(*outGain);
 
    updateDistortType(apvts, tb);
}

WaveShaperComp::~WaveShaperComp()
{

}

void WaveShaperComp::paint(juce::Graphics& g)
{

}

void WaveShaperComp::resized()
{
    auto bounds = getLocalBounds();

    auto bigKnob = bounds.removeFromTop(bounds.getHeight() * .8);
    auto bigRight = bigKnob.removeFromRight(bigKnob.getWidth() * .5);
    select->setBounds(bigKnob);
    distort->setBounds(bigRight);

    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .5);

    inGain->setBounds(leftSide);
    outGain->setBounds(rightSide);
    mix->setBounds(bounds);
}

void WaveShaperComp::updateAttachments(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    inGainAT.reset();
    selectAT.reset();
    distortAT.reset();
    mixAT.reset();
    outGainAT.reset();

    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);

    enum Pos
    {
        Type,
        Sin,
        Quad,
        Factor,
        GB,
        In,
        Mix,
        Out
    };

    Pos pos;

    auto& inGainParam = getParam(apvts, params, bandNames.at(Pos::In));
    auto& selectParam = getParam(apvts, params, bandNames.at(Pos::Type));

    auto range = selectParam.getNormalisableRange();

    auto correctParam = juce::jmap(selectParam.getValue(), range.start, range.end);
    if (correctParam == 0)
        pos = Pos::Sin;
    else if (correctParam == 1)
        pos = Pos::Quad;
    else if (correctParam == 2)
        pos = Pos::Factor;
    else
        pos = Pos::GB;

    auto& distortParam = getParam(apvts, params, bandNames.at(pos));
    auto& mixParam = getParam(apvts, params, bandNames.at(Pos::Mix));
    auto& outGainParam = getParam(apvts, params, bandNames.at(Pos::Out));

    inGain.get()->changeParam(&inGainParam);
    select.get()->changeParam(&selectParam);
    distort.get()->changeParam(&distortParam);
    mix.get()->changeParam(&mixParam);
    outGain.get()->changeParam(&outGainParam);

    makeAttachment(inGainAT, apvts, params, bandNames.at(Pos::In), *inGain);
    makeAttachment(selectAT, apvts, params, bandNames.at(Pos::Type), *select);
    makeAttachment(distortAT, apvts, params, bandNames.at(pos), *distort);
    makeAttachment(mixAT, apvts, params, bandNames.at(Pos::Mix), *mix);
    makeAttachment(outGainAT, apvts, params, bandNames.at(Pos::Out), *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
    addLabelPairs(select->labels, 1, 3, selectParam, "", 20, typeText);
    addLabelPairs(distort->labels, 1, 3, distortParam, "", 20);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
        };
    select.get()->onValueChange = [this, &selectParam, &apvts, &tb]()
        {
            addLabelPairs(select->labels, 1, 3, selectParam, "", 20, typeText);
            updateDistortType(apvts, tb);
        };
    mix.get()->onValueChange = [this, &mixParam]()
        {
            addLabelPairs(mix->labels, 1, 3, mixParam, "%");
        };
    outGain.get()->onValueChange = [this, &outGainParam]()
        {
            addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");
        };
}


void WaveShaperComp::updateRSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);

    enum Pos
    {
        Type,
        Sin,
        holder1,
        holder2,
        holder3,
        In,
        Mix,
        Out
    };

    auto& inGainParam = getParam(apvts, params, bandNames.at(Pos::In));
    auto& selectParam = getParam(apvts, params, bandNames.at(Pos::Type));
    auto& distortParm = getParam(apvts, params, bandNames.at(Pos::Sin));
    auto& mixParam = getParam(apvts, params, bandNames.at(Pos::Mix));
    auto& outGainParam = getParam(apvts, params, bandNames.at(Pos::Out));

    inGain = std::make_unique<RotarySliderWithLabels>(&inGainParam, "dB", "In Gain");
    select = std::make_unique<RotarySliderWithLabels>(&selectParam, "", "Select");
    distort = std::make_unique<RotarySliderWithLabels>(&distortParm, "", "Distortion Factor");
    mix = std::make_unique<RotarySliderWithLabels>(&mixParam, "%", "Mix");
    outGain = std::make_unique<RotarySliderWithLabels>(&outGainParam, "dB", "Out Gain");

    makeAttachment(inGainAT, apvts, params, bandNames.at(Pos::In), *inGain);
    makeAttachment(selectAT, apvts, params, bandNames.at(Pos::Type), *select);
    makeAttachment(distortAT, apvts, params, bandNames.at(Pos::Sin), *distort);
    makeAttachment(mixAT, apvts, params, bandNames.at(Pos::Mix), *mix);
    makeAttachment(outGainAT, apvts, params, bandNames.at(Pos::Out), *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
    addLabelPairs(select->labels, 1, 3, selectParam, "", 20, typeText);
    addLabelPairs(distort->labels, 1, 3, distortParm, "", 20);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
        };
    select.get()->onValueChange = [this, &selectParam, &apvts, &tb]()
        {
            addLabelPairs(select->labels, 1, 3, selectParam, "", 20, typeText);
            updateDistortType(apvts, tb);
        };
    mix.get()->onValueChange = [this, &mixParam]()
        {
            addLabelPairs(mix->labels, 1, 3, mixParam, "%");
        };
    outGain.get()->onValueChange = [this, &outGainParam]()
        {
            addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");
        };
}

void WaveShaperComp::updateDistortType(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);

    enum Pos
    {
        holder1,
        Sin,
        Quad,
        Factor,
        GB,
        holder2,
        holder3,
        holder4
    };

    distortAT.reset();
    auto newID = bandNames.at(Pos::Sin);
    auto param = select.get()->getValue();

    if (param == 0)
    {
        newID = bandNames.at(Pos::Sin);
    }
    else if (param == 1)
    {
        newID = bandNames.at(Pos::Quad);
    }
    else if (param == 2)
    {
        newID = bandNames.at(Pos::Factor);
    }
    else
    {
        newID = bandNames.at(Pos::GB);
    }

    
    auto& distortParm = getParam(apvts, params, newID);
    distort.get()->changeParam(&distortParm);
    makeAttachment(distortAT, apvts, params, newID, *distort);
    addLabelPairs(distort->labels, 1, 3, distortParm, "", 20);

    distort.get()->onValueChange = [this, &distortParm]()
        {
            addLabelPairs(distort->labels, 1, 3, distortParm, "", 20);
        };
    
}

void WaveShaperComp::updateSelectLabels()
{

}

std::vector<Params::names> WaveShaperComp::getParamsAtBand(juce::String& band)
{
    std::vector<Params::names> Names;

    if (band == "Low")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Waveshaper_Three_Type,
            Params::names::Waveshaper_Three_Drive_Sin,
            Params::names::Waveshaper_Three_Drive_Quad,
            Params::names::Waveshaper_Three_Drive_Factor,
            Params::names::Waveshaper_Three_Drive_GB,
            Params::names::Waveshaper_Three_In,
            Params::names::Waveshaper_Three_Mix,
            Params::names::Waveshaper_Three_Out
        };
    }
    else if (band == "Mid")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Waveshaper_Two_Type,
            Params::names::Waveshaper_Two_Drive_Sin,
            Params::names::Waveshaper_Two_Drive_Quad,
            Params::names::Waveshaper_Two_Drive_Factor,
            Params::names::Waveshaper_Two_Drive_GB,
            Params::names::Waveshaper_Two_In,
            Params::names::Waveshaper_Two_Mix,
            Params::names::Waveshaper_Two_Out
        };
    }

    else if(band == "High")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Waveshaper_One_Type,
            Params::names::Waveshaper_One_Drive_Sin,
            Params::names::Waveshaper_One_Drive_Quad,
            Params::names::Waveshaper_One_Drive_Factor,
            Params::names::Waveshaper_One_Drive_GB,
            Params::names::Waveshaper_One_In,
            Params::names::Waveshaper_One_Mix,
            Params::names::Waveshaper_One_Out
        };
    }

    else
    {
        Names = std::vector<Params::names>
        {
            Params::names::Waveshaper_Single_Type,
            Params::names::Waveshaper_Single_Drive_Sin,
            Params::names::Waveshaper_Single_Drive_Quad,
            Params::names::Waveshaper_Single_Drive_Factor,
            Params::names::Waveshaper_Single_Drive_GB,
            Params::names::Waveshaper_Single_In,
            Params::names::Waveshaper_Single_Mix,
            Params::names::Waveshaper_Single_Out
        };
    }

    return Names;
}
