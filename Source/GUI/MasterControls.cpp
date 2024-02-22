/*
  ==============================================================================

    MasterControls.cpp
    Created: 17 Nov 2023 4:39:37pm
    Author:  kylew

  ==============================================================================
*/

#include "MasterControls.h"

MasterComp::MasterComp(Mangl3rAudioProcessor& p) :
    audioP(p),
    bypassAT(p.apvts, "Global Bypass", bypass)

{
    updateSWL(p.apvts);

    addAndMakeVisible(meter[0]);
    addAndMakeVisible(meter[1]);
    addAndMakeVisible(outMeter[0]);
    addAndMakeVisible(outMeter[1]);
    
    addAndMakeVisible(*mix);
    addAndMakeVisible(*inGain);
    addAndMakeVisible(*outGain);
    addAndMakeVisible(bypass);
    bypass.setComponentID("Global");

    startTimerHz(24);
}

MasterComp::~MasterComp()
{

}

void MasterComp::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto meterBounds = bounds.reduced(0, bounds.getHeight() * .1);
    meterBounds.translate(0, -bounds.getHeight() * .025);
    auto topBounds = bounds.removeFromTop(bounds.getHeight() * .07);
    auto bottomBounds = bounds.removeFromBottom(bounds.getHeight() * .11);
    g.setColour(juce::Colour(64u, 194u, 230u));

    juce::Line<int> line;

    line.setStart(meterBounds.getBottomLeft());
    line.setEnd(meterBounds.getBottomRight());
    g.drawLine(line.toFloat(), 1.f);

    line.setStart(topBounds.getBottomLeft());
    line.setEnd(topBounds.getBottomRight());
    g.drawLine(line.toFloat(), 1.f);

    g.setColour(juce::Colours::white);
    auto leftRMS = topBounds.reduced(topBounds.getWidth() * .4, 0);
    leftRMS.translate(-topBounds.getWidth() * .3, 0);
    
    auto leftMeters = meterBounds.reduced(bounds.getWidth() * .4, 0);
    leftMeters.translate(-meterBounds.getWidth() * .3, 0);

    auto input = audioP.levelMeterData.getRMS(0);
    auto output = audioP.levelMeterData.getOutRMS(0);

    auto leftMetersbounds = meterBounds.reduced(bounds.getWidth() * .4, 0);
    leftMetersbounds.translate(-meterBounds.getWidth() * .35, 0);

    auto rightMetersBounds = meterBounds.reduced(bounds.getWidth() * .4, 0);
    rightMetersBounds.translate(meterBounds.getWidth() * .35, 0);

    g.drawFittedText((juce::String)input, leftMetersbounds, juce::Justification::centredTop, 1);
    g.drawFittedText((juce::String)output, rightMetersBounds, juce::Justification::centredTop, 1);
    
    //auto newFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::OFFSHORE_TTF, BinaryData::OFFSHORE_TTFSize));
    g.setFont(newFont);
    g.setFont(topBounds.getHeight() * .95);
    g.drawFittedText("DistorK", topBounds, juce::Justification::centred, 1);
}

void MasterComp::resized()
{
    auto bounds = getLocalBounds();
    auto meterBounds = bounds.reduced(0, bounds.getHeight() * .1);
    meterBounds.translate(0, -bounds.getHeight() * .025);
    auto topBounds = bounds.removeFromTop(bounds.getHeight() * .07);
    auto bottomBounds = bounds.removeFromBottom(bounds.getHeight() * .15);
    bottomBounds.reduce(bottomBounds.getWidth() * .05, 0);

    auto leftMeters = meterBounds.reduced(bounds.getWidth() * .4, 0);
    leftMeters.translate(-meterBounds.getWidth() * .35, 0);
    auto leftMetersRight = leftMeters.removeFromRight(leftMeters.getWidth() * .5);

    auto rightMeters = meterBounds.reduced(bounds.getWidth() * .4, 0);
    rightMeters.translate(meterBounds.getWidth() * .35, 0);
    auto rightMetersRight = rightMeters.removeFromRight(rightMeters.getWidth() * .5);

    auto inGainArea = meterBounds.reduced(bounds.getWidth() * .38, 0);
    inGainArea.removeFromTop(meterBounds.getHeight() * .1);
    inGainArea.translate(-meterBounds.getWidth() * .125, -5);
    inGain->setBounds(inGainArea);

    auto outGainArea = meterBounds.reduced(bounds.getWidth() * .38, 0);
    outGainArea.removeFromTop(meterBounds.getHeight() * .1);
    outGainArea.translate(meterBounds.getWidth() * .125, -5);
    outGain->setBounds(outGainArea);

    auto bypassArea = meterBounds.reduced(bounds.getWidth() * .3, 0);
    bypassArea.removeFromBottom(bypassArea.getHeight() * .95);
    bypass.setBounds(bypassArea);

    meter[0].setBounds(leftMeters);
    meter[1].setBounds(leftMetersRight);
    outMeter[0].setBounds(rightMeters);
    outMeter[1].setBounds(rightMetersRight);
    mix->setBounds(bottomBounds);
}

void MasterComp::timerCallback()
{
    for (auto channel = 0; channel < audioP.getTotalNumInputChannels(); channel++) {
        meter[channel].setLevel(audioP.levelMeterData.getRMS(channel));

        outMeter[channel].setLevel(audioP.levelMeterData.getOutRMS(channel));
    }

    repaint();

}

void MasterComp::updateSWL(juce::AudioProcessorValueTreeState& apvts)
{
    const auto& params = Params::getParams();

    auto& inGainParam = getSliderParam(apvts, params, Params::names::Master_In_Gain);
    auto& mixParam = getSliderParam(apvts, params, Params::names::Master_Mix);
    auto& outGainParam = getSliderParam(apvts, params, Params::names::Master_Out_Gain);

    inGain = std::make_unique<SliderWithLabels>(&inGainParam, "In Gain", juce::Slider::LinearVertical);
    mix = std::make_unique<SliderWithLabels>(&mixParam, "Dry/Wet", juce::Slider::LinearHorizontal);
    outGain = std::make_unique<SliderWithLabels>(&outGainParam, "Out Gain", juce::Slider::LinearVertical);
    
    makeSliderAttachment(inGainAT, apvts, params, Params::names::Master_In_Gain, *inGain);
    makeSliderAttachment(mixAT, apvts, params, Params::names::Master_Mix, *mix);
    makeSliderAttachment(outGainAT, apvts, params, Params::names::Master_Out_Gain, *outGain);

    addLabelPairs(inGain->labels, inGainParam, " dB");
    addLabelPairs(mix->labels, mixParam, "%");
    addLabelPairs(outGain->labels, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, inGainParam, " dB");
        };

    outGain.get()->onValueChange = [this, &outGainParam]()
        {
            addLabelPairs(outGain->labels, outGainParam, " dB");
        };

    mix.get()->onValueChange = [this, &mixParam]()
        {
            addLabelPairs(mix->labels, mixParam, "%");
        };
}
