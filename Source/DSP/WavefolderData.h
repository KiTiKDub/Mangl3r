/*
  ==============================================================================

    WavefolderData.h
    Created: 14 Feb 2024 4:13:29pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utility/overSampleGain.h"

struct Wavefolder
{
    Wavefolder() {};
    virtual ~Wavefolder() {};

    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block);
    void updateParams(bool bypass, int typeSelect, std::vector<juce::AudioParameterFloat*>& factors, float inGain, float outGain, float mix);

private:

    overSampleGain osg;

    bool wavefolderBypass{ false };
    int wavefolderTypeSelect{ 0 };
    std::vector<juce::AudioParameterFloat*> wavefolderFactors;
    float wavefolderInGainValue{ 0.f };
    float wavefolderOutGainValue{ 0.f };
    float wavefolderMix{ 0.f };
    float sampleRate{ 44100.f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Wavefolder)
};