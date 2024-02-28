/*
  ==============================================================================

    BitCrusher.cpp
    Created: 17 Nov 2023 4:40:15pm
    Author:  kylew

  ==============================================================================
*/

#include "BitCrusher.h"

BitCrusherComp::BitCrusherComp(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    updateRSWL(apvts, tb);

    addAndMakeVisible(*rate);
    addAndMakeVisible(*depth);
    addAndMakeVisible(*inGain);
    addAndMakeVisible(*mix);
    addAndMakeVisible(*outGain);
}

BitCrusherComp::~BitCrusherComp()
{

}

void BitCrusherComp::paint(juce::Graphics& g)
{

}

void BitCrusherComp::resized()
{
    auto bounds = getLocalBounds();

    auto bigKnob = bounds.removeFromTop(bounds.getHeight() * .8);
    auto bigRight = bigKnob.removeFromRight(bigKnob.getWidth() * .5);
    rate->setBounds(bigKnob);
    depth->setBounds(bigRight);

    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .5);

    inGain->setBounds(leftSide);
    outGain->setBounds(rightSide);
    mix->setBounds(bounds);
}

void BitCrusherComp::updateAttachments(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    inGainAT.reset();
    rateAT.reset();
    depthAT.reset();
    mixAT.reset();
    outGainAT.reset();

    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);

    enum Pos
    {
        Depth,
        Rate,
        In,
        Mix,
        Out
    };

    auto& inGainParam = getParam(apvts, params, bandNames.at(Pos::In));
    auto& rateParam = getParam(apvts, params, bandNames.at(Pos::Rate));
    auto& depthParam = getParam(apvts, params, bandNames.at(Pos::Depth));
    auto& mixParam = getParam(apvts, params, bandNames.at(Pos::Mix));
    auto& outGainParam = getParam(apvts, params, bandNames.at(Pos::Out));

    inGain.get()->changeParam(&inGainParam);
    rate.get()->changeParam(&rateParam);
    depth.get()->changeParam(&depthParam);
    mix.get()->changeParam(&mixParam);
    outGain.get()->changeParam(&outGainParam);

    makeAttachment(inGainAT, apvts, params, bandNames.at(Pos::In), *inGain);
    makeAttachment(rateAT, apvts, params, bandNames.at(Pos::Rate), *rate);
    makeAttachment(depthAT, apvts, params, bandNames.at(Pos::Depth), *depth);
    makeAttachment(mixAT, apvts, params, bandNames.at(Pos::Mix), *mix);
    makeAttachment(outGainAT, apvts, params, bandNames.at(Pos::Out), *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
    addLabelPairs(rate->labels, 1, 3, rateParam, "", 20);
    addLabelPairs(depth->labels, 1, 3, depthParam, "", 20);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
        };
    rate.get()->onValueChange = [this, &rateParam]()
        {
            addLabelPairs(rate->labels, 1, 3, rateParam, "", 20);
        };
    depth.get()->onValueChange = [this, &depthParam]()
        {
            addLabelPairs(depth->labels, 1, 3, depthParam, "", 20);
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

std::vector<Params::names> BitCrusherComp::getParamsAtBand(juce::String& band)
{

    std::vector<Params::names> Names;

    if (band == "Low")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Bitcrusher_Three_Depth,
            Params::names::Bitcrusher_Three_Rate,
            Params::names::Bitcrusher_Three_In,
            Params::names::Bitcrusher_Three_Mix,
            Params::names::Bitcrusher_Three_Out
        };
    }
    else if (band == "Mid")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Bitcrusher_Two_Depth,
            Params::names::Bitcrusher_Two_Rate,
            Params::names::Bitcrusher_Two_In,
            Params::names::Bitcrusher_Two_Mix,
            Params::names::Bitcrusher_Two_Out
        };
    }

    else if(band == "High")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Bitcrusher_One_Depth,
            Params::names::Bitcrusher_One_Rate,
            Params::names::Bitcrusher_One_In,
            Params::names::Bitcrusher_One_Mix,
            Params::names::Bitcrusher_One_Out
        };
    }

    else
    {
        Names = std::vector<Params::names>
        {
            Params::names::Bitcrusher_Single_Depth,
            Params::names::Bitcrusher_Single_Rate,
            Params::names::Bitcrusher_Single_In,
            Params::names::Bitcrusher_Single_Mix,
            Params::names::Bitcrusher_Single_Out
        };
    }

    return Names;
}

void BitCrusherComp::updateRSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);

    enum Pos
    {
        Depth,
        Rate,
        In,
        Mix,
        Out
    };

    auto& inGainParam = getParam(apvts, params, bandNames.at(Pos::In));
    auto& rateParam = getParam(apvts, params, bandNames.at(Pos::Rate));
    auto& depthParam = getParam(apvts, params, bandNames.at(Pos::Depth));
    auto& mixParam = getParam(apvts, params, bandNames.at(Pos::Mix));
    auto& outGainParam = getParam(apvts, params, bandNames.at(Pos::Out));

    inGain = std::make_unique<RotarySliderWithLabels>(&inGainParam, "dB", "In Gain");
    rate = std::make_unique<RotarySliderWithLabels>(&rateParam, "", "Bit Rate");
    depth = std::make_unique<RotarySliderWithLabels>(&depthParam, "", "Bit Depth");
    mix = std::make_unique<RotarySliderWithLabels>(&mixParam, "%", "Mix");
    outGain = std::make_unique<RotarySliderWithLabels>(&outGainParam, "dB", "Out Gain");

    makeAttachment(inGainAT, apvts, params, bandNames.at(Pos::In), *inGain);
    makeAttachment(rateAT, apvts, params, bandNames.at(Pos::Rate), *rate);
    makeAttachment(depthAT, apvts, params, bandNames.at(Pos::Depth), *depth);
    makeAttachment(mixAT, apvts, params, bandNames.at(Pos::Mix), *mix);
    makeAttachment(outGainAT, apvts, params, bandNames.at(Pos::Out), *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB"); 
    addLabelPairs(rate->labels, 1, 3, rateParam, "", 20);
    addLabelPairs(depth->labels, 1, 3, depthParam, "", 20);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
        };
    rate.get()->onValueChange = [this, &rateParam]()
        {
            addLabelPairs(rate->labels, 1, 3, rateParam, "", 20);
        };
    depth.get()->onValueChange = [this, &depthParam]()
        {
            addLabelPairs(depth->labels, 1, 3, depthParam, "", 20);
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
