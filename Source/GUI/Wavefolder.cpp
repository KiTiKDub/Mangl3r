/*
  ==============================================================================

    Wavefolder.cpp
    Created: 14 Feb 2024 4:13:41pm
    Author:  kylew

  ==============================================================================
*/

#include "Wavefolder.h"

WavefolderComp::WavefolderComp(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    updateRSWL(apvts, tb);

    addAndMakeVisible(*select);
    addAndMakeVisible(*distort);
    addAndMakeVisible(*inGain);
    addAndMakeVisible(*mix);
    addAndMakeVisible(*outGain);

    updateDistortType(apvts, tb);
}

WavefolderComp::~WavefolderComp()
{

}

void WavefolderComp::paint(juce::Graphics& g)
{

}

void WavefolderComp::resized()
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

void WavefolderComp::updateAttachments(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    //auto selectParam = select.get()->getValue();

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
        Tri,
        In,
        Mix,
        Out
    };

    Pos pos;

    auto& inGainParam = getParam(apvts, params, bandNames.at(Pos::In));
    auto& selectParam = getParam(apvts, params, bandNames.at(Pos::Type));

    auto range = selectParam.getNormalisableRange();

    auto correctType = juce::jmap(selectParam.getValue(), range.start, range.end);
    if (correctType == 0)
        pos = Pos::Sin;
    else
        pos = Pos::Tri;

    auto& distortParm = getParam(apvts, params, bandNames.at(pos));
    auto& mixParam = getParam(apvts, params, bandNames.at(Pos::Mix));
    auto& outGainParam = getParam(apvts, params, bandNames.at(Pos::Out));

    inGain.get()->changeParam(&inGainParam);
    select.get()->changeParam(&selectParam);
    distort.get()->changeParam(&distortParm);
    mix.get()->changeParam(&mixParam);
    outGain.get()->changeParam(&outGainParam);

    makeAttachment(inGainAT, apvts, params, bandNames.at(Pos::In), *inGain);
    makeAttachment(selectAT, apvts, params, bandNames.at(Pos::Type), *select);
    makeAttachment(distortAT, apvts, params, bandNames.at(pos), *distort);
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
    distort.get()->onValueChange = [this, &distortParm]()
    {
        addLabelPairs(distort->labels, 1, 3, distortParm, "", 20);
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

void WavefolderComp::updateRSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);

    enum Pos
    {
        Type,
        Sin,
        holder1,
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
    distort.get()->onValueChange = [this, &distortParm]()
    {
        addLabelPairs(distort->labels, 1, 3, distortParm, "", 20);
    };
    select.get()->onValueChange = [this, &selectParam, &apvts, &tb]()
        {
            addLabelPairs(select->labels, 1, 3, selectParam, "", 20, typeText);
            // updateDistortType(apvts, tb);
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

void WavefolderComp::updateDistortType(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);

    enum Pos
    {
        holder1,
        Sin,
        Tri,
        holder2,
        holder3,
        holder4
    };

    distortAT.reset();
    Params::names newID;
    auto param = select.get()->getValue();

    if (param == 0)
    {
        newID = bandNames.at(Pos::Sin);
    }
    else if (param == 1)
    {
        newID = bandNames.at(Pos::Tri);
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

std::vector<Params::names> WavefolderComp::getParamsAtBand(juce::String& band)
{
    std::vector<Params::names> Names;

    if (band == "Low")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Wavefolder_One_Type,
            Params::names::Wavefolder_One_Drive_Sin,
            Params::names::WaveFolder_One_Drive_Tri,
            Params::names::Wavefolder_One_In,
            Params::names::Wavefolder_One_Mix,
            Params::names::Wavefolder_One_Out
        };
    }
    else if (band == "Mid")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Wavefolder_Two_Type,
            Params::names::Wavefolder_Two_Drive_Sin,
            Params::names::WaveFolder_Two_Drive_Tri,
            Params::names::Wavefolder_Two_In,
            Params::names::Wavefolder_Two_Mix,
            Params::names::Wavefolder_Two_Out
        };
    }

    else if(band == "High")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Wavefolder_Three_Type,
            Params::names::Wavefolder_Three_Drive_Sin,
            Params::names::WaveFolder_Three_Drive_Tri,
            Params::names::Wavefolder_Three_In,
            Params::names::Wavefolder_Three_Mix,
            Params::names::Wavefolder_Three_Out
        };
    }

    else
    {
        Names = std::vector<Params::names>
        {
            Params::names::Wavefolder_Single_Type,
            Params::names::Wavefolder_Single_Drive_Sin,
            Params::names::WaveFolder_Single_Drive_Tri,
            Params::names::Wavefolder_Single_In,
            Params::names::Wavefolder_Single_Mix,
            Params::names::Wavefolder_Single_Out
        };
    }

    return Names;
}
