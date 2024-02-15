/*
  ==============================================================================

    Wavefolder.h
    Created: 14 Feb 2024 4:13:41pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "rotarySliderWithLabels.h"
#include "../Utility/Params.h"
#include "Toolbar.h"

struct WavefolderComp : public juce::Component
{
    WavefolderComp(juce::AudioProcessorValueTreeState& apvts, ToolbarComp&);
    ~WavefolderComp();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void updateAttachments(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb);

private:
    void updateRSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb);
    void updateDistortType(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb);
    std::vector<Params::names> getParamsAtBand(juce::String&);

    std::unique_ptr<RotarySliderWithLabels> select, distort, inGain, mix, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> selectAT, distortAT, inGainAT, mixAT, outGainAT;

    std::vector<juce::String> typeText{ "Sine", "Triangle" };
};