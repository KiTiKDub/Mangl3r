/*
  ==============================================================================

    Clipper.cpp
    Created: 17 Nov 2023 4:40:46pm
    Author:  kylew

  ==============================================================================
*/

#include "Clipper.h"

ClipperComp::ClipperComp(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    updateRSWL(apvts, tb);
    updateSWL(apvts, tb);

    addAndMakeVisible(*select);
    addAndMakeVisible(*threshold);
    addAndMakeVisible(*inGain);
    addAndMakeVisible(*mix);
    addAndMakeVisible(*outGain);
}

ClipperComp::~ClipperComp()
{

}

void ClipperComp::paint(juce::Graphics& g)
{

}

void ClipperComp::resized()
{
    auto bounds = getLocalBounds();

    auto bigKnob = bounds.removeFromTop(bounds.getHeight() * .8);
    auto sliderSpace = bigKnob.removeFromRight(bigKnob.getWidth() * .2);
    sliderSpace.reduce(0, sliderSpace.getHeight() * .1);
    sliderSpace.removeFromBottom(sliderSpace.getHeight() * .05);
    threshold->setBounds(sliderSpace);
    select->setBounds(bigKnob);

    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .5);

    inGain->setBounds(leftSide);
    outGain->setBounds(rightSide);
    mix->setBounds(bounds);
}

void ClipperComp::updateRSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);

    enum Pos
    {
        Type,
        Threshold,
        In,
        Mix,
        Out
    };

    auto& inGainParam = getParam(apvts, params, bandNames.at(Pos::In));
    auto& selectParam = getParam(apvts, params, bandNames.at(Pos::Type));
    auto& mixParam = getParam(apvts, params, bandNames.at(Pos::Mix));
    auto& outGainParam = getParam(apvts, params, bandNames.at(Pos::Out));

    inGain = std::make_unique<RotarySliderWithLabels>(&inGainParam, "dB", "In Gain");
    select = std::make_unique<RotarySliderWithLabels>(&selectParam, "", "Select");
    mix = std::make_unique<RotarySliderWithLabels>(&mixParam, "%", "Mix");
    outGain = std::make_unique<RotarySliderWithLabels>(&outGainParam, "dB", "Out Gain");

    makeAttachment(inGainAT, apvts, params, bandNames.at(Pos::In), *inGain);
    makeAttachment(selectAT, apvts, params, bandNames.at(Pos::Type), *select);
    makeAttachment(mixAT, apvts, params, bandNames.at(Pos::Mix), *mix);
    makeAttachment(outGainAT, apvts, params, bandNames.at(Pos::Out), *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
    addLabelPairs(select->labels, 1, 3, selectParam, "", 24, typeText);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
        };
    select.get()->onValueChange = [this, &selectParam]()
        {
            addLabelPairs(select->labels, 1, 3, selectParam, "", 24, typeText);
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

void ClipperComp::updateSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);

    enum Pos
    {
        Type,
        Threshold,
        In,
        Mix,
        Out
    };

    auto& thresholdParm = getSliderParam(apvts, params, bandNames.at(Pos::Threshold));
    threshold = std::make_unique<SliderWithLabels>(&thresholdParm, "Threshold", juce::Slider::LinearVertical);
    makeSliderAttachment(thresholdAT, apvts, params, bandNames.at(Pos::Threshold), *threshold);
    addLabelPairs(threshold->labels, thresholdParm, " dB");

    threshold.get()->onValueChange = [this, &thresholdParm]()
        {
            addLabelPairs(threshold->labels, thresholdParm, " dB");
        };
}

std::vector<Params::names> ClipperComp::getParamsAtBand(juce::String& band)
{
    std::vector<Params::names> Names;

    if (band == "Low")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Clipper_Three_Type,
            Params::names::Clipper_Three_Threshold,
            Params::names::Clipper_Three_In,
            Params::names::Clipper_Three_Mix,
            Params::names::Clipper_Three_Out
        };
    }
    else if (band == "Mid")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Clipper_Two_Type,
            Params::names::Clipper_Two_Threshold,
            Params::names::Clipper_Two_In,
            Params::names::Clipper_Two_Mix,
            Params::names::Clipper_Two_Out
        };
    }

    else
    {
        Names = std::vector<Params::names>
        {
            Params::names::Clipper_One_Type,
            Params::names::Clipper_One_Threshold,
            Params::names::Clipper_One_In,
            Params::names::Clipper_One_Mix,
            Params::names::Clipper_One_Out
        };
    }

    return Names;
}
