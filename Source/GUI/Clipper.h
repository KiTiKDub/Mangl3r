/*
  ==============================================================================

    Clipper.h
    Created: 17 Nov 2023 4:40:46pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "rotarySliderWithLabels.h"
#include "SliderWithLabels.h"
#include "../Utility/Params.h"
#include "Toolbar.h"

struct ClipperComp : public juce::Component
{
    ClipperComp(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb);
    ~ClipperComp();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void updateAttachments(juce::AudioProcessorValueTreeState& apvts, ToolbarComp&);

private:

    void updateSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp&);
    void updateRSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp&);
    std::vector<Params::names> getParamsAtBand(juce::String&);

    std::unique_ptr<RotarySliderWithLabels> select, inGain, mix, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> selectAT, inGainAT, mixAT, outGainAT;

    std::unique_ptr<SliderWithLabels> threshold;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAT;

    std::vector<juce::String> typeText{ "Hard", "Soft", "Sine", "Hyperbolic Tan", "Arc Tan", "Quintic"};
};