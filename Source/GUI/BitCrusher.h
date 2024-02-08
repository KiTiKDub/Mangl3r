/*
  ==============================================================================

    BitCrusher.h
    Created: 17 Nov 2023 4:40:15pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "rotarySliderWithLabels.h"
#include "../Utility/Params.h"
#include "Toolbar.h"

struct BitCrusherComp : public juce::Component
{
    BitCrusherComp(juce::AudioProcessorValueTreeState& apvts, ToolbarComp&);
    ~BitCrusherComp();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void updateAttachments(juce::AudioProcessorValueTreeState& apvts, ToolbarComp&);

private:

    void updateRSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp&);
    std::vector<Params::names> getParamsAtBand(juce::String&);

    std::unique_ptr<RotarySliderWithLabels> depth, rate, inGain, mix, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAT, rateAT, inGainAT, mixAT, outGainAT;
};