/*
  ==============================================================================

    DistorkEngine.h
    Created: 26 Jan 2024 7:02:01am
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "BitCrusherData.h"
#include "clipperData.h"
#include "SaturationData.h"
#include "WaveShaperData.h"
#include "WavefolderData.h"

struct DistorkEngine
{
    void prepareToPlay(juce::dsp::ProcessSpec&);
    void process(juce::dsp::AudioBlock<float>&, std::vector<int>, int);

    juce::AudioParameterInt* clipperSelect{ nullptr };
    juce::AudioParameterFloat* clipperThresh{ nullptr };
    juce::AudioParameterFloat* clipperInGain{ nullptr };
    juce::AudioParameterFloat* clipperOutGain{ nullptr };
    juce::AudioParameterFloat* clipperMix{ nullptr };
    juce::AudioParameterBool* clipperToggle{ nullptr };

    juce::AudioParameterInt* waveShaperSelect{ nullptr };
    juce::AudioParameterFloat* waveShaperSin{ nullptr };
    juce::AudioParameterFloat* waveShaperQuadratic{ nullptr };
    juce::AudioParameterFloat* waveShaperFactor{ nullptr };
    juce::AudioParameterFloat* waveShaperGB{ nullptr };
    juce::AudioParameterFloat* waveShaperInGain{ nullptr };
    juce::AudioParameterFloat* waveShaperOutGain{ nullptr };
    juce::AudioParameterFloat* waveShaperMix{ nullptr };
    juce::AudioParameterBool* waveShaperToggle{ nullptr };
    std::vector<juce::AudioParameterFloat*> waveShaperFactorsHolder{ waveShaperSin, waveShaperQuadratic, waveShaperFactor, waveShaperGB };

    juce::AudioParameterInt* crusherBitDepth{ nullptr };
    juce::AudioParameterInt* crusherBitRate{ nullptr };
    juce::AudioParameterFloat* crusherInGain{ nullptr };
    juce::AudioParameterFloat* crusherOutGain{ nullptr };
    juce::AudioParameterFloat* crusherMix{ nullptr };
    juce::AudioParameterBool* crusherToggle{ nullptr };

    juce::AudioParameterFloat* satDrive{ nullptr };
    juce::AudioParameterFloat* satInGain{ nullptr };
    juce::AudioParameterFloat* satOutGain{ nullptr };
    juce::AudioParameterFloat* satMix{ nullptr };
    juce::AudioParameterBool* satToggle{ nullptr };

    juce::AudioParameterInt* wavefolderSelect{ nullptr };
    juce::AudioParameterFloat* wavefolderSin{ nullptr };
    juce::AudioParameterFloat* wavefolderTri{ nullptr };
    juce::AudioParameterFloat* wavefolderInGain{ nullptr };
    juce::AudioParameterFloat* wavefolderOutGain{ nullptr };
    juce::AudioParameterFloat* wavefolderMix{ nullptr };
    juce::AudioParameterBool* wavefolderToggle{ nullptr };
    std::vector<juce::AudioParameterFloat*> wavefolderFactorsHolder{ wavefolderSin, wavefolderTri };

private:
    BitCrusher bitcrusher;
    Clipper clipper;
    Saturation saturator;
    WaveShaper waveshaper;
    Wavefolder wavefolder;

    void updateParams();
};