/*
  ==============================================================================

    Analyzer.h
    Created: 6 Feb 2024 5:39:12pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utility/KiTiK_utilityViz.h"
#include "../PluginProcessor.h"
#include "../GUI/rotarySliderWithLabels.h"
#include "../Utility/Params.h"

struct AnalyzerComp : public juce::Component
{
    AnalyzerComp(Mangl3rAudioProcessor&);
    ~AnalyzerComp();

    void resized() override;
    void paint(juce::Graphics&) override;

    void updateRSWL(juce::AudioProcessorValueTreeState& apvts);
    void update();

private:

    void drawCrossovers(juce::Graphics&, juce::Rectangle<int>&);

    FFTComp fftComp;

    std::unique_ptr<RotarySliderWithLabels> lowMid, midHigh;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidAT, midHighAT;

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };
};