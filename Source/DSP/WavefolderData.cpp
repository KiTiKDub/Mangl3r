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
    inGain.reset();
    inGain.prepare(spec);
    inGain.setRampDurationSeconds(.05);

    outGain.reset();
    outGain.prepare(spec);
    outGain.setRampDurationSeconds(.05);

    sampleRate = spec.sampleRate;
}

void Wavefolder::process(juce::dsp::AudioBlock<float>& block, int ch)
{
    if (!wavefolderBypass) { return; };

    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    inGain.setGainDecibels(wavefolderInGainValue);
    inGain.process(context);

    auto* channelInput = context.getInputBlock().getChannelPointer(ch);
    auto* channelOutput = context.getOutputBlock().getChannelPointer(ch);

    if (wavefolderTypeSelect == 0)
    {
        for (int s = 0; s < context.getInputBlock().getNumSamples(); s++)
        {
            auto output = std::sinf(juce::MathConstants<float>::twoPi * channelInput[s] * wavefolderFactors[0]->get() / 2);

            channelOutput[s] = output * wavefolderMix + (1-wavefolderMix) * channelInput[s];
        }
    }
    else
    {
        for (int s = 0; s < context.getInputBlock().getNumSamples(); s++)
        {
            auto data = channelInput[s] * wavefolderFactors[1]->get();
            auto output = 4 * (std::abs(.25 * data + .25 - std::round(.25 * data + .25)) - .25);
            channelOutput[s] = output * wavefolderMix + (1 - wavefolderMix) * channelInput[s];
        }
    }

    outGain.setGainDecibels(wavefolderOutGainValue);
    outGain.process(context);
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
