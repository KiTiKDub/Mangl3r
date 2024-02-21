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

    using namespace Params;
    const auto& paramNames = getParams();

    auto floatHelper = [&apvts = ap.apvts, &paramNames](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(paramNames.at(paramName)));
        };

    floatHelper(lowMidCrossover, names::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossover, names::Mid_High_Crossover_Freq);
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
    auto border = paintArea;
    border.setBottom(paintArea.getBottom() + 2);

    g.setColour(juce::Colours::whitesmoke);
    g.fillRoundedRectangle(paintArea.toFloat(), 2);
    g.setColour(/*juce::Colour(186u, 34u, 34u)*/juce::Colour(64u, 194u, 230u));
    ;
    g.fillRect(paintArea.getX() - 2, paintArea.getY(), 2, paintArea.getHeight());
    g.fillRect(paintArea.getRight(), paintArea.getY(), 2, paintArea.getHeight());
    g.fillRect(paintArea.getX() - 2, paintArea.getY() - 2, paintArea.getWidth() + 4, 2);

    drawCrossovers(g, paintArea);
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

void AnalyzerComp::drawCrossovers(juce::Graphics& g, juce::Rectangle<int>& r)
{
    using namespace juce;

    auto bounds = r;

    const auto top = bounds.getY();
    const auto bottom = bounds.getBottom();

    auto mapX = [left = bounds.getX(), width = bounds.getWidth()](float frequency)
        {
            auto normX = mapFromLog10(frequency, 20.f, 20000.f);

            return left + width * normX;
        };

    auto lowMidx = mapX(lowMidCrossover->get());
    g.setColour(Colours::blue);
    g.drawVerticalLine(lowMidx, top, bottom);

    auto midHighx = mapX(midHighCrossover->get());
    g.setColour(Colours::blue);
    g.drawVerticalLine(midHighx, top, bottom);

}
