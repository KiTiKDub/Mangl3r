/*
  ==============================================================================

    DistorkEngine.cpp
    Created: 26 Jan 2024 7:02:01am
    Author:  kylew

  ==============================================================================
*/

#include "DistorkEngine.h"

void DistorkEngine::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
    saturator.prepareToPlay(spec);
    clipper.prepareToPlay(spec);
    waveshaper.prepareToPlay(spec);
    bitcrusher.prepareToPlay(spec);
}

void DistorkEngine::process(juce::dsp::AudioBlock<float>& block, int channel)
{
    updateParams();

    saturator.process(block, channel);
    clipper.process(block, channel);
    waveshaper.process(block, channel);
    bitcrusher.process(block, channel);
}

void DistorkEngine::updateParams()
{
    saturator.updateParams(satToggle->get(), satDrive->get(), satInGain->get(), satOutGain->get(), satMix->get());
    clipper.updateParams(clipperToggle->get(), clipperSelect->get(), clipperThresh->get(), clipperInGain->get(), clipperOutGain->get(), clipperMix->get());
    waveshaper.updateParams(waveShaperToggle->get(), waveShaperSelect->get(), waveShaperFactorsHolder, waveShaperInGain->get(), waveShaperOutGain->get(), waveShaperMix->get());
    bitcrusher.updateParams(crusherToggle->get(), crusherBitDepth->get(), crusherBitRate->get(), crusherInGain->get(), crusherOutGain->get(), crusherMix->get());
}
