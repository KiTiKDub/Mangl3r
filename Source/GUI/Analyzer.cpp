/*
  ==============================================================================

    Analyzer.cpp
    Created: 6 Feb 2024 5:39:12pm
    Author:  kylew

  ==============================================================================
*/

#include "Analyzer.h"

AnalyzerComp::AnalyzerComp(Mangl3rAudioProcessor& ap) : fftComp(ap.fftData)
{
    updateRSWL(ap.apvts);

    addAndMakeVisible(fftComp);
    addAndMakeVisible(*lowMid);
    addAndMakeVisible(*midHigh);
}

AnalyzerComp::~AnalyzerComp()
{
}

void AnalyzerComp::resized()
{
    auto bounds = getLocalBounds();
    auto paintArea = bounds.reduced(bounds.getWidth() * .1, bounds.getHeight() * .1);

    auto sliderSide = bounds.removeFromLeft(bounds.getWidth() * .1);
    auto midHighArea = sliderSide.removeFromTop(sliderSide.getHeight() * .5);

    midHigh->setBounds(midHighArea);
    lowMid->setBounds(sliderSide);

    fftComp.setBounds(paintArea);
}

void AnalyzerComp::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto paintArea = bounds.reduced(bounds.getWidth() * .1, bounds.getHeight() * .1);

    g.setColour(juce::Colours::whitesmoke);
    g.fillRoundedRectangle(paintArea.toFloat(), 2);
}

void AnalyzerComp::updateRSWL(juce::AudioProcessorValueTreeState& apvts)
{
    const auto& params = Params::getParams();

    auto& lowMidParam = getParam(apvts, params, Params::names::Low_Mid_Crossover_Freq);
    auto& midHighParam = getParam(apvts, params, Params::names::Mid_High_Crossover_Freq);

    lowMid = std::make_unique<RotarySliderWithLabels>(&lowMidParam, "Hz", "Low Mid X-Over Freq");
    midHigh = std::make_unique<RotarySliderWithLabels>(&midHighParam, "Hz", "Mid High X-Over Freq");

    makeAttachment(lowMidAT, apvts, params, Params::names::Low_Mid_Crossover_Freq, *lowMid);
    makeAttachment(midHighAT, apvts, params, Params::names::Mid_High_Crossover_Freq, *midHigh);

    addLabelPairs(lowMid->labels, 1, 3, lowMidParam, "Hz");
    addLabelPairs(midHigh->labels, 1, 3, midHighParam, "Hz");

    lowMid.get()->onValueChange = [this, &lowMidParam]()
        {
            addLabelPairs(lowMid->labels, 1, 3, lowMidParam, "Hz");
        };

    midHigh.get()->onValueChange = [this, &midHighParam]()
        {
            addLabelPairs(lowMid->labels, 1, 3, midHighParam, "Hz");
        };
}

void AnalyzerComp::update()
{
    fftComp.repaint();
}
