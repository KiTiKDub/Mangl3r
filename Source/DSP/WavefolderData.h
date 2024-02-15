/*
  ==============================================================================

    WavefolderData.h
    Created: 14 Feb 2024 4:13:29pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct Wavefolder
{
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block, int ch);
    void updateParams(bool bypass, int typeSelect, std::vector<juce::AudioParameterFloat*>& factors, float inGain, float outGain, float mix);

private:

    juce::dsp::Gain<float> inGain;
    juce::dsp::Gain<float> outGain;

    bool wavefolderBypass;
    int wavefolderTypeSelect;
    std::vector<juce::AudioParameterFloat*> wavefolderFactors;
    float wavefolderInGainValue;
    float wavefolderOutGainValue;
    float wavefolderMix;
    float sampleRate;
};