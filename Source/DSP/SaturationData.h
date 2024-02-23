/*
  ==============================================================================

    SaturationData.h
    Created: 17 Nov 2023 4:43:11pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct Saturation
{
    Saturation() {};
    virtual ~Saturation() {};

    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block, int channel);
    void updateParams(bool bypass, float drive, float inGain, float outGain, float mix);

private:

    juce::dsp::Gain<float> inGain;
    juce::dsp::Gain<float> outGain;

    bool satBypass{ false };
    float satDrive{ 0.f };
    float satInGain{ 0.f };
    float satOutGain{ 0.f };
    float satMix{ 0.f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Saturation)
};
