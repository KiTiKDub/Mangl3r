/*
  ==============================================================================

    WaveShaper.h
    Created: 17 Nov 2023 4:40:32pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "rotarySliderWithLabels.h"
#include "../Utility/Params.h"
#include "Toolbar.h"


struct WaveShaperComp : public juce::Component
{
    WaveShaperComp(juce::AudioProcessorValueTreeState& apvts, ToolbarComp&);
    ~WaveShaperComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:

    void updateRSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb);
    void updateAttachments(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb);
    void updateSelectLabels();
    std::vector<Params::names> getParamsAtBand(juce::String&);

    std::unique_ptr<RotarySliderWithLabels> select, distort, inGain, mix, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> selectAT, distortAT, inGainAT, mixAT, outGainAT;

    std::vector<juce::String> typeText{ "Sine", "Quadratic", "Factor", "Gloubi Boulga" };
};