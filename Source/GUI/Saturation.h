/*
  ==============================================================================

    Saturation.h
    Created: 17 Nov 2023 4:41:53pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "rotarySliderWithLabels.h"
#include "../Utility/Params.h"
#include "Toolbar.h"

struct SaturationComp : public juce::Component
{
    SaturationComp(juce::AudioProcessorValueTreeState& apvts, ToolbarComp&);
    ~SaturationComp();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void updateAttachments(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb);

private:

    void updateRSWL(juce::AudioProcessorValueTreeState& apvts, ToolbarComp& tb);
    std::vector<Params::names> getParamsAtBand(juce::String&);

    std::unique_ptr<RotarySliderWithLabels> drive, inGain, outGain, mix;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAT, inGainAT, outGainAT, mixAT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaturationComp)
};