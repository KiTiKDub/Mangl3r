/*
  ==============================================================================

    Saturation.cpp
    Created: 17 Nov 2023 4:41:53pm
    Author:  kylew

  ==============================================================================
*/

#include "Saturation.h"

#define NORMAL_FONT_SIZE 14

SaturationComp::SaturationComp(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    updateRSWL(apvts, tb);

    addAndMakeVisible(*inGain);
    addAndMakeVisible(*drive);
    addAndMakeVisible(*mix);
    addAndMakeVisible(*outGain);

}

SaturationComp::~SaturationComp()
{

}

void SaturationComp::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::white);

    auto bigKnob = bounds.removeFromTop(bounds.getHeight() * .7);

    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .5);

}

void SaturationComp::resized()
{
    auto bounds = getLocalBounds();

    auto bigKnob = bounds.removeFromTop(bounds.getHeight() * .8);
    drive->setBounds(bigKnob);

    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .5);

    inGain->setBounds(leftSide);
    outGain->setBounds(rightSide);
    mix->setBounds(bounds);

}

void SaturationComp::updateRSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);
    
    enum Pos 
    {
        Drive,
        In,
        Mix,
        Out
    };

    auto& inGainParam = getParam(apvts, params, bandNames.at(Pos::In));
    auto& driveParam = getParam(apvts, params, bandNames.at(Pos::Drive));
    auto& mixParam = getParam(apvts, params, bandNames.at(Pos::Mix));
    auto& outGainParam = getParam(apvts, params, bandNames.at(Pos::Out));

    inGain = std::make_unique<RotarySliderWithLabels>(&inGainParam, "dB", "In Gain");
    drive = std::make_unique<RotarySliderWithLabels>(&driveParam, "", "Drive");
    mix = std::make_unique<RotarySliderWithLabels>(&mixParam, "%", "Mix");
    outGain = std::make_unique<RotarySliderWithLabels>(&outGainParam, "dB", "Out Gain");

    makeAttachment(inGainAT, apvts, params, bandNames.at(Pos::In), *inGain);
    makeAttachment(driveAT, apvts, params, bandNames.at(Pos::Drive), *drive);
    makeAttachment(mixAT, apvts, params, bandNames.at(Pos::Mix), *mix);
    makeAttachment(outGainAT, apvts, params, bandNames.at(Pos::Out), *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB"); //lets add default values here instead of using constants
    addLabelPairs(drive->labels, 1, 3, driveParam, "", 24);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
        };
    drive.get()->onValueChange = [this, &driveParam]()
        {
            addLabelPairs(drive->labels, 1, 3, driveParam, "", 24);
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

void SaturationComp::updateAttachments(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb)
{
    inGainAT.reset();
    driveAT.reset();
    mixAT.reset();
    outGainAT.reset();

    const auto& params = Params::getParams();

    auto band = tb.getActiveBand();
    auto bandNames = getParamsAtBand(band);

    enum Pos
    {
        Drive,
        In,
        Mix,
        Out
    };

    auto& inGainParam = getParam(apvts, params, bandNames.at(Pos::In));
    auto& driveParam = getParam(apvts, params, bandNames.at(Pos::Drive));
    auto& mixParam = getParam(apvts, params, bandNames.at(Pos::Mix));
    auto& outGainParam = getParam(apvts, params, bandNames.at(Pos::Out));

    inGain.get()->changeParam(&inGainParam);
    drive.get()->changeParam(&driveParam);
    mix.get()->changeParam(&mixParam);
    outGain.get()->changeParam(&outGainParam);

    makeAttachment(inGainAT, apvts, params, bandNames.at(Pos::In), *inGain);
    makeAttachment(driveAT, apvts, params, bandNames.at(Pos::Drive), *drive);
    makeAttachment(mixAT, apvts, params, bandNames.at(Pos::Mix), *mix);
    makeAttachment(outGainAT, apvts, params, bandNames.at(Pos::Out), *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB"); //lets add default values here instead of using constants
    addLabelPairs(drive->labels, 1, 3, driveParam, "", 24);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
        };
    drive.get()->onValueChange = [this, &driveParam]()
        {
            addLabelPairs(drive->labels, 1, 3, driveParam, "", 24);
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

std::vector<Params::names> SaturationComp::getParamsAtBand(juce::String& band)
{
    std::vector<Params::names> Names;

    if (band == "Low")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Saturator_Three_Drive,
            Params::names::Saturator_Three_In,
            Params::names::Saturator_Three_Mix,
            Params::names::Saturator_Three_Out,
        };
    }
    else if(band == "Mid")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Saturator_Two_Drive,
            Params::names::Saturator_Two_In,
            Params::names::Saturator_Two_Mix,
            Params::names::Saturator_Two_Out
        };
    }

    else if(band == "High")
    {
        Names = std::vector<Params::names>
        {
            Params::names::Saturator_One_Drive,
            Params::names::Saturator_One_In,
            Params::names::Saturator_One_Mix,
            Params::names::Saturator_One_Out,
        };
    }

    else
    {
        Names = std::vector<Params::names>
        {
            Params::names::Saturator_Single_Drive,
            Params::names::Saturator_Single_In,
            Params::names::Saturator_Single_Mix,
            Params::names::Saturator_Single_Out,
        };
    }

    return Names;
}
