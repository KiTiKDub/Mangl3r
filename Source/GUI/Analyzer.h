/*
  ==============================================================================

    Analyzer.h
    Created: 6 Feb 2024 5:39:12pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
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

    juce::ToggleButton lowBP, lowMute, midBP, midMute, highBP, highMute;
    juce::AudioProcessorValueTreeState::ButtonAttachment lowBPAT, lowMuteAT, midBPAT, midMuteAT, highBPAT, highMuteAT;

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyzerComp)
};
