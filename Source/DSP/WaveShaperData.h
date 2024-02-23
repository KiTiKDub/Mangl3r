/*
  ==============================================================================

    WaveShaperData.h
    Created: 17 Nov 2023 4:42:05pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
struct WaveShaper 
{
    WaveShaper() {};
    virtual ~WaveShaper() {} ;

    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block, int channel);
    void updateParams(bool bypass, int typeSelect, std::vector<juce::AudioParameterFloat*>& factors, float inGain, float outGain, float mix);

private:

    void processSinusoidal(int channel, juce::dsp::ProcessContextReplacing<float>& context);
    void processQuadratic(int channel, juce::dsp::ProcessContextReplacing<float>& context);
    void processFactor(int channel, juce::dsp::ProcessContextReplacing<float>& context);
    void processGB(int channel, juce::dsp::ProcessContextReplacing<float>& context);

    enum WaveShaperSelect {
        sinusoidal,
        quadratic,
        factor,
        GloubiBoulga
    };

    juce::dsp::Gain<float> inGain;
    juce::dsp::Gain<float> outGain;

    bool waveShaperBypass{ false };
    int waveShaperTypeSelect{ 0 };
    std::vector<juce::AudioParameterFloat*> waveShaperFactors{ nullptr };
    float waveShaperInGainValue{ 0.f };
    float waveShaperOutGainValue{ 0.f };
    float waveshaperMix{ 0.f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveShaper)
};