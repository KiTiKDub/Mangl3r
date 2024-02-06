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

struct DistorkEngine
{
    juce::AudioParameterInt* clipperSelect{ nullptr };
    juce::AudioParameterFloat* clipperThresh{ nullptr };
    juce::AudioParameterFloat* clipperInGain{ nullptr };
    juce::AudioParameterFloat* clipperOutGain{ nullptr };
    juce::AudioParameterFloat* clipperMix{ nullptr };

    juce::AudioParameterInt* waveShaperSelect{ nullptr };
    juce::AudioParameterFloat* waveShaperSin{ nullptr };
    juce::AudioParameterFloat* waveShaperQuadratic{ nullptr };
    juce::AudioParameterFloat* waveShaperFactor{ nullptr };
    juce::AudioParameterFloat* waveShaperGB{ nullptr };
    juce::AudioParameterFloat* waveShaperInGain{ nullptr };
    juce::AudioParameterFloat* waveShaperOutGain{ nullptr };
    juce::AudioParameterFloat* waveShaperMix{ nullptr };
    std::vector<juce::AudioParameterFloat*> waveShaperFactorsHolder{ waveShaperSin, waveShaperQuadratic, waveShaperFactor, waveShaperGB };

    juce::AudioParameterInt* crusherBitDepth{ nullptr };
    juce::AudioParameterInt* crusherBitRate{ nullptr };
    juce::AudioParameterFloat* crusherInGain{ nullptr };
    juce::AudioParameterFloat* crusherOutGain{ nullptr };
    juce::AudioParameterFloat* crusherMix{ nullptr };

    juce::AudioParameterFloat* satDrive{ nullptr };
    juce::AudioParameterFloat* satInGain{ nullptr };
    juce::AudioParameterFloat* satOutGain{ nullptr };
    juce::AudioParameterFloat* satMix{ nullptr };
    juce::AudioParameterBool* satToggle{ nullptr };

    BitCrusher bitcrusher;
    Clipper clipper;
    Saturation saturator;
    WaveShaper waveshaper;
};