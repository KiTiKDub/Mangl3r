/*
  ==============================================================================

    BitCrusherData.h
    Created: 17 Nov 2023 4:42:41pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct BitCrusher
{
    BitCrusher() {};
    virtual ~BitCrusher() {};

    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block, int channel);
    void updateParams(bool bypass, int bitDepth, int bitRate, float inGain, float outGain, float mix);

private:
    
    juce::dsp::Gain<float> inGain;
    juce::dsp::Gain<float> outGain;

    bool crusherBypass{ false };
    int crusherBitDepth{ 0 };
    int crusherBitRate{ 0 };
    float crusherInGain{ 0.f };
    float crusherOutGain{ 0.f };
    float crusherMix{ 0.f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BitCrusher)
};