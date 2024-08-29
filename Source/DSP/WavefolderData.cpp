/*
  ==============================================================================

    WavefolderData.cpp
    Created: 14 Feb 2024 4:13:29pm
    Author:  kylew

  ==============================================================================
*/

#include "WavefolderData.h"

void Wavefolder::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
}

void Wavefolder::process(juce::dsp::AudioBlock<float>& block)
{
    if (!wavefolderBypass) { return; };


    auto len = block.getNumSamples();

    for(int channel = 0; channel < block.getNumChannels(); channel++)
    {
        osg.process(block, wavefolderInGainValue, channel);
        
        auto data = block.getChannelPointer(channel);

        if (wavefolderTypeSelect == 0)
        {
            for (int s = 0; s < len; s++)
            {
                auto output = std::sinf(juce::MathConstants<float>::twoPi * data[s] * wavefolderFactors[0]->get() / 2);

                data[s] = output * wavefolderMix + (1 - wavefolderMix) * data[s];
            }
        }
        else
        {
            for (int s = 0; s < len; s++)
            {
                auto factor = data[s] * wavefolderFactors[1]->get();
                auto output = 4 * (std::abs(.25 * factor + .25 - std::round(.25 * factor + .25)) - .25);
                data[s] = output * wavefolderMix + (1 - wavefolderMix) * data[s];
            }
        }

        osg.process(block, wavefolderOutGainValue, channel);
    }
}

void Wavefolder::updateParams(bool bypass, int typeSelect, std::vector<juce::AudioParameterFloat*>& factors, float inGain, float outGain, float mix)
{
    wavefolderBypass = bypass;
    wavefolderTypeSelect = typeSelect;
    wavefolderFactors = factors;
    wavefolderInGainValue = inGain;
    wavefolderMix = mix/100;
    wavefolderOutGainValue = outGain;
}
