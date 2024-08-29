/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Mangl3rAudioProcessor::Mangl3rAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    apvts.state.setProperty(PresetManager::presetNameProperty, "", nullptr);
    apvts.state.setProperty("version", ProjectInfo::versionString, nullptr);

    presetManager = std::make_unique<PresetManager>(apvts);

    using namespace Params;
    const auto& params = getParams();

    singleBandMode = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Single_Band_Mode)));

    globalBypass = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Global_Bypass)));
    masterInValue = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Master_In_Gain)));
    masterMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Master_Mix)));
    masterOutValue = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Master_Out_Gain)));

    lowMidCrossover = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Low_Mid_Crossover_Freq)));
    midHighCrossover = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Mid_High_Crossover_Freq)));

    selectToolbarOne = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Select_Toolbar_One)));
    selectToolbarTwo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Select_Toolbar_Two)));
    selectToolbarThree = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Select_Toolbar_Three)));
    oversampleRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Oversample_Rate)));

    lowMute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Engine_Three_Mute)));
    midMute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Engine_Two_Mute)));
    highMute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Engine_One_Mute)));

    engineHigh.engineToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Engine_One_Toggle)));
    engineMid.engineToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Engine_Two_Toggle)));
    engineLow.engineToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Engine_Three_Toggle)));
    engineSingle.engineToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Engine_Single_Toggle)));

    engineHigh.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_In)));
    engineHigh.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_Mix)));
    engineHigh.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_Out)));
    engineHigh.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Clipper_One_Type)));
    engineHigh.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_Threshold)));
    engineHigh.clipperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Clipper_One_Toggle)));

    engineHigh.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_One_Depth)));
    engineHigh.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_One_Rate)));
    engineHigh.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_One_In)));
    engineHigh.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_One_Mix)));
    engineHigh.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_One_Out)));
    engineHigh.crusherToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Bitcrusher_One_Toggle)));

    engineHigh.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_Drive)));
    engineHigh.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_In)));
    engineHigh.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_Mix)));
    engineHigh.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_Out)));
    engineHigh.satToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Saturator_One_Toggle)));

    engineHigh.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Waveshaper_One_Type)));
    engineHigh.waveShaperFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_Sin)));
    engineHigh.waveShaperFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_Quad)));
    engineHigh.waveShaperFactorsHolder[2] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_Factor)));
    engineHigh.waveShaperFactorsHolder[3] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_GB)));
    engineHigh.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_In)));
    engineHigh.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Mix)));
    engineHigh.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Out)));
    engineHigh.waveShaperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Waveshaper_One_Toggle)));

    engineHigh.wavefolderSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Wavefolder_One_Type)));
    engineHigh.wavefolderFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_One_Drive_Sin)));
    engineHigh.wavefolderFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::WaveFolder_One_Drive_Tri)));
    engineHigh.wavefolderInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_One_In)));
    engineHigh.wavefolderMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_One_Mix)));
    engineHigh.wavefolderOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_One_Out)));
    engineHigh.wavefolderToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Wavefolder_One_Toggle)));

    engineMid.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_In)));
    engineMid.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_Mix)));
    engineMid.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_Out)));
    engineMid.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Clipper_Two_Type)));
    engineMid.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_Threshold)));
    engineMid.clipperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Clipper_Two_Toggle)));

    engineMid.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Depth)));
    engineMid.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Rate)));
    engineMid.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Two_In)));
    engineMid.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Mix)));
    engineMid.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Out)));
    engineMid.crusherToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Toggle)));

    engineMid.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_Drive)));
    engineMid.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_In)));
    engineMid.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_Mix)));
    engineMid.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_Out)));
    engineMid.satToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Saturator_Two_Toggle)));

    engineMid.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Waveshaper_Two_Type)));
    engineMid.waveShaperFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_Sin)));
    engineMid.waveShaperFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_Quad)));
    engineMid.waveShaperFactorsHolder[2] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_Factor)));
    engineMid.waveShaperFactorsHolder[3] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_GB)));
    engineMid.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_In)));
    engineMid.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Mix)));
    engineMid.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Out)));
    engineMid.waveShaperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Waveshaper_Two_Toggle)));

    engineMid.wavefolderSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Wavefolder_Two_Type)));
    engineMid.wavefolderFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Two_Drive_Sin)));
    engineMid.wavefolderFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::WaveFolder_Two_Drive_Tri)));
    engineMid.wavefolderInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Two_In)));
    engineMid.wavefolderMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Two_Mix)));
    engineMid.wavefolderOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Two_Out)));
    engineMid.wavefolderToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Wavefolder_Two_Toggle)));

    engineLow.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_In)));
    engineLow.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_Mix)));
    engineLow.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_Out)));
    engineLow.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Clipper_Three_Type)));
    engineLow.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_Threshold)));
    engineLow.clipperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Clipper_Three_Toggle)));

    engineLow.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Depth)));
    engineLow.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Rate)));
    engineLow.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Three_In)));
    engineLow.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Mix)));
    engineLow.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Out)));
    engineLow.crusherToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Toggle)));

    engineLow.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_Drive)));
    engineLow.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_In)));
    engineLow.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_Mix)));
    engineLow.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_Out)));
    engineLow.satToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Saturator_Three_Toggle)));

    engineLow.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Waveshaper_Three_Type)));
    engineLow.waveShaperFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_Sin)));
    engineLow.waveShaperFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_Quad)));
    engineLow.waveShaperFactorsHolder[2] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_Factor)));
    engineLow.waveShaperFactorsHolder[3] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_GB)));
    engineLow.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_In)));
    engineLow.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Mix)));
    engineLow.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Out)));
    engineLow.waveShaperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Waveshaper_Three_Toggle)));

    engineLow.wavefolderSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Wavefolder_Three_Type)));
    engineLow.wavefolderFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Three_Drive_Sin)));
    engineLow.wavefolderFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::WaveFolder_Three_Drive_Tri)));
    engineLow.wavefolderInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Three_In)));
    engineLow.wavefolderMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Three_Mix)));
    engineLow.wavefolderOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Three_Out)));
    engineLow.wavefolderToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Wavefolder_Three_Toggle)));

    engineSingle.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Single_In)));
    engineSingle.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Single_Mix)));
    engineSingle.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Single_Out)));
    engineSingle.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Clipper_Single_Type)));
    engineSingle.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Single_Threshold)));
    engineSingle.clipperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Clipper_Single_Toggle)));

    engineSingle.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Single_Depth)));
    engineSingle.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Single_Rate)));
    engineSingle.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Single_In)));
    engineSingle.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Single_Mix)));
    engineSingle.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Single_Out)));
    engineSingle.crusherToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Bitcrusher_Single_Toggle)));

    engineSingle.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Single_Drive)));
    engineSingle.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Single_In)));
    engineSingle.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Single_Mix)));
    engineSingle.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Single_Out)));
    engineSingle.satToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Saturator_Single_Toggle)));

    engineSingle.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Waveshaper_Single_Type)));
    engineSingle.waveShaperFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Single_Drive_Sin)));
    engineSingle.waveShaperFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Single_Drive_Quad)));
    engineSingle.waveShaperFactorsHolder[2] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Single_Drive_Factor)));
    engineSingle.waveShaperFactorsHolder[3] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Single_Drive_GB)));
    engineSingle.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Single_In)));
    engineSingle.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Single_Mix)));
    engineSingle.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Single_Out)));
    engineSingle.waveShaperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Waveshaper_Single_Toggle)));

    engineSingle.wavefolderSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Wavefolder_Single_Type)));
    engineSingle.wavefolderFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Single_Drive_Sin)));
    engineSingle.wavefolderFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::WaveFolder_Single_Drive_Tri)));
    engineSingle.wavefolderInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Single_In)));
    engineSingle.wavefolderMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Single_Mix)));
    engineSingle.wavefolderOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Single_Out)));
    engineSingle.wavefolderToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Wavefolder_Single_Toggle)));

}

Mangl3rAudioProcessor::~Mangl3rAudioProcessor()
{
}

//==============================================================================
const juce::String Mangl3rAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Mangl3rAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Mangl3rAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Mangl3rAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Mangl3rAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Mangl3rAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Mangl3rAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Mangl3rAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Mangl3rAudioProcessor::getProgramName (int index)
{
    return {};
}

void Mangl3rAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Mangl3rAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    masterIn.reset();
    masterIn.prepare(spec);
    masterIn.setRampDurationSeconds(.05);
    
    masterOut.reset();
    masterOut.prepare(spec);
    masterOut.setRampDurationSeconds(.05);

    fftData.prepare(sampleRate);

    engineHigh.prepareToPlay(spec);
    engineMid.prepareToPlay(spec);
    engineLow.prepareToPlay(spec);
    engineSingle.prepareToPlay(spec);

    LP1.reset();
    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    LP1.prepare(spec);
    HP1.reset();
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    HP1.prepare(spec);

    AP2.reset();
    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    AP2.prepare(spec);

    LP2.reset();
    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    LP2.prepare(spec);
    HP2.reset();
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    HP2.prepare(spec);

    for (auto& buffer : filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }

    for (auto& oversample : overSamplers)
    {
        oversample.reset();
        oversample.initProcessing(samplesPerBlock);
    }

    //osc.initialise([](float x) {return std::sin(x); }); //FFT graph testing
    //osc.prepare(spec);
    //osc.setFrequency(200);

    //gain.prepare(spec);
    //gain.setGainDecibels(-12.f);
}

void Mangl3rAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Mangl3rAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Mangl3rAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (!globalBypass->get()) { return; }

    juce::AudioBuffer<float> copyBuffer;
    copyBuffer = buffer;

    auto singleBlock = juce::dsp::AudioBlock<float>(buffer);
    auto singleContext = juce::dsp::ProcessContextReplacing<float>(singleBlock);

    masterIn.setGainDecibels(masterInValue->get());
    masterIn.process(singleContext);

    levelMeterData.process(true, 0, buffer);
    levelMeterData.process(true, 1, buffer);

    if(singleBandMode->get())
    {
        auto lowMidCutoff = lowMidCrossover->get();
        LP1.setCutoffFrequency(lowMidCutoff);
        HP1.setCutoffFrequency(lowMidCutoff);

        auto midHighCutoff = midHighCrossover->get();
        AP2.setCutoffFrequency(midHighCutoff);
        LP2.setCutoffFrequency(midHighCutoff);
        HP2.setCutoffFrequency(midHighCutoff);

        //if (false) //Spectrum analyzer testing
        //{
        //    buffer.clear();
        //    auto block = juce::dsp::AudioBlock<float>(buffer);
        //    auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        //    osc.process(ctx);
        //    osc.setFrequency(JUCE_LIVE_CONSTANT(200));

        //    gain.process(ctx);
        //}

        for (auto& fb : filterBuffers)
        {
            fb = buffer;
        }

        auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
        auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
        auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);

        auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
        auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
        auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);

        LP1.process(fb0Ctx);
        AP2.process(fb0Ctx);

        HP1.process(fb1Ctx);
        filterBuffers[2] = filterBuffers[1];
        LP2.process(fb1Ctx);

        HP2.process(fb2Ctx);

        auto ovRate = oversampleRate->get();

        auto ov0Block = overSamplers[ovRate].processSamplesUp(fb0Ctx.getInputBlock());
        auto ov1Block = overSamplers[ovRate + 4].processSamplesUp(fb1Ctx.getInputBlock());
        auto ov2Block = overSamplers[ovRate + 8].processSamplesUp(fb2Ctx.getInputBlock());

        for (int ch = 0; ch < totalNumInputChannels; ch++)
        {
            engineHigh.process(ov2Block, toolbarHighOrder, ch);
            engineMid.process(ov1Block, toolbarMidOrder, ch);
            engineLow.process(ov0Block, toolbarLowOrder, ch);
        }

        overSamplers[ovRate].processSamplesDown(fb0Ctx.getOutputBlock());
        overSamplers[ovRate + 4].processSamplesDown(fb1Ctx.getOutputBlock());
        overSamplers[ovRate + 8].processSamplesDown(fb2Ctx.getOutputBlock());

        auto numSamples = buffer.getNumSamples();

        buffer.clear();

        auto addFilterBand = [nc = getTotalNumOutputChannels(), ns = numSamples](auto& inputBuffer, const auto& source)
            {
                for (auto i = 0; i < nc; i++)
                {
                    inputBuffer.addFrom(i, 0, source, i, 0, ns);
                };
            };

        if (!lowMute->get())
            addFilterBand(buffer, filterBuffers[0]);
        if (!midMute->get())
            addFilterBand(buffer, filterBuffers[1]);
        if (!highMute->get())
            addFilterBand(buffer, filterBuffers[2]);
    }
    
    else
    {
        auto ovRate = oversampleRate->get();

        auto ovBlock = overSamplers[ovRate].processSamplesUp(singleContext.getInputBlock());

        for (int ch = 0; ch < totalNumInputChannels; ch++)
        {
            engineSingle.process(ovBlock, toolbarSingleOrder, ch);
        }

        overSamplers[ovRate].processSamplesDown(singleContext.getOutputBlock());
    }

    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        auto data = buffer.getWritePointer(ch);
        auto dry = copyBuffer.getReadPointer(ch);

        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            data[s] = (data[s] * masterMix->get() / 100) + (dry[s] * (1 - masterMix->get() / 100));
        }
    }

    masterOut.setGainDecibels(masterOutValue->get());
    masterOut.process(singleContext);

    fftData.pushNextSampleIntoFifo(buffer);

    levelMeterData.process(false, 0, buffer);
    levelMeterData.process(false, 1, buffer);
}

//==============================================================================
bool Mangl3rAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Mangl3rAudioProcessor::createEditor()
{
    return new Mangl3rAudioProcessorEditor (*this);
}

//==============================================================================
void Mangl3rAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    const auto state = apvts.copyState();
    const auto xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Mangl3rAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState == nullptr)
        return;
    const auto newTree = juce::ValueTree::fromXml(*xmlState);
    apvts.replaceState(newTree);
}

juce::AudioProcessorValueTreeState::ParameterLayout Mangl3rAudioProcessor::createParameterLayout()
{
    using namespace juce;
    using namespace Params;
    const auto& params = getParams();

    AudioProcessorValueTreeState::ParameterLayout layout;

    auto gainRange = NormalisableRange<float>(-24, 24, .1, 1);
    auto mixRange = NormalisableRange<float>(0, 100, 1, 1);
    auto lowMidRange = NormalisableRange<float>(20, 999, 1, 1);
    auto midHighRange = NormalisableRange<float>(1000, 20000, 1, 1);

    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Single_Band_Mode), 1}, "Single", true));

    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Global_Bypass), 1}, "Global Bypass", true));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Master_In_Gain), 1}, "In", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Master_Out_Gain), 1}, "Out", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Master_Mix), 1}, "Mix", mixRange, 100));

    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Select_Toolbar_One),1}, "Toolbar One", true));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Select_Toolbar_Two),1}, "Toolbar Two", false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Select_Toolbar_Three),1}, "Toolbar Three", false));

    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Low_Mid_Crossover_Freq),1}, "Low Xover", lowMidRange, 400));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Mid_High_Crossover_Freq),1}, "High Xover", midHighRange, 2000));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Oversample_Rate),1}, "OS Rate", 0, 3, 0));

    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Engine_One_Toggle),1}, "Bypass", false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Engine_Two_Toggle),1}, "Bypass", false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Engine_Three_Toggle),1}, "Bypass", false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Engine_Single_Toggle),1}, "Bypass", false));

    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Engine_One_Mute),1}, "Mute", false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Engine_Two_Mute),1}, "Mute", false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Engine_Three_Mute),1}, "Mute", false));


    //Clipper Controls
    auto threshRange = NormalisableRange<float>(-60, 0, .1, 1);
    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Clipper_One_Type),1}, "Clipper Type", 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_One_Threshold),1}, "Threshold", threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_One_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_One_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_One_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Clipper_One_Toggle),1}, params.at(names::Clipper_One_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Clipper_Two_Type),1}, "Clipper Type", 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Two_Threshold),1}, "Threshold", threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Two_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Two_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Two_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Clipper_Two_Toggle),1}, params.at(names::Clipper_Two_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Clipper_Three_Type),1}, "Clipper Type", 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Three_Threshold),1}, "Threshold", threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Three_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Three_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Three_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Clipper_Three_Toggle),1}, params.at(names::Clipper_Three_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Clipper_Single_Type),1}, "Clipper Type", 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Single_Threshold),1}, "Threshold", threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Single_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Single_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Clipper_Single_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Clipper_Single_Toggle),1}, params.at(names::Clipper_Single_Toggle), true));

    //WaveShaper Controls
    auto lessThanOne = NormalisableRange<float>(.01, .99, .01, 1);
    auto sineFactor = NormalisableRange<float>(.05, .95, .01, 1);
    auto moreThanOne = NormalisableRange<float>(1, 10, .01, 1);
    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Waveshaper_One_Type),1}, "Type", 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_One_Drive_Sin),1}, "Drive", sineFactor, .05));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_One_Drive_Quad),1}, "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_One_Drive_Factor),1}, "Drive", lessThanOne, .05));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_One_Drive_GB),1}, "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_One_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_One_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_One_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Waveshaper_One_Toggle),1}, params.at(names::Waveshaper_One_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Waveshaper_Two_Type),1}, "Type", 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Two_Drive_Sin),1}, "Drive", sineFactor, .05));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Two_Drive_Quad),1}, "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Two_Drive_Factor),1}, "Drive", lessThanOne, .05));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Two_Drive_GB),1}, "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Two_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Two_Mix),1}, "Dry/Wet", mixRange, 1000));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Two_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Waveshaper_Two_Toggle),1}, params.at(names::Waveshaper_Two_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Waveshaper_Three_Type),1}, "Type", 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Three_Drive_Sin),1}, "Drive", sineFactor, .05));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Three_Drive_Quad),1}, "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Three_Drive_Factor),1}, "Drive", lessThanOne, .05));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Three_Drive_GB),1}, "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Three_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Three_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Three_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Waveshaper_Three_Toggle),1}, params.at(names::Waveshaper_Three_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Waveshaper_Single_Type),1}, "Type", 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Single_Drive_Sin),1}, "Drive", sineFactor, .05));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Single_Drive_Quad),1}, "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Single_Drive_Factor),1}, "Drive", lessThanOne, .05));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Single_Drive_GB),1}, "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Single_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Single_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Waveshaper_Single_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Waveshaper_Single_Toggle),1}, params.at(names::Waveshaper_Single_Toggle), true));

    //BitCrusher Controls
    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Bitcrusher_One_Depth),1}, "Bit Depth", 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Bitcrusher_One_Rate),1}, "Bit Rate", 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_One_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_One_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_One_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Bitcrusher_One_Toggle),1}, params.at(names::Bitcrusher_One_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Bitcrusher_Two_Depth),1}, "Bit Depth", 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Bitcrusher_Two_Rate),1}, "Bit Rate", 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_Two_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_Two_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_Two_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Bitcrusher_Two_Toggle),1}, params.at(names::Bitcrusher_Two_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Bitcrusher_Three_Depth),1}, "Bit Depth", 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Bitcrusher_Three_Rate),1}, "Bit Rate", 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_Three_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_Three_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_Three_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Bitcrusher_Three_Toggle),1}, params.at(names::Bitcrusher_Three_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Bitcrusher_Single_Depth),1}, "Bit Depth", 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Bitcrusher_Single_Rate),1}, "Bit Rate", 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_Single_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_Single_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Bitcrusher_Single_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Bitcrusher_Single_Toggle),1}, params.at(names::Bitcrusher_Single_Toggle), true));

    //Saturation Controls
    auto driveRange = NormalisableRange<float>(1, 10, .1, 1);
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_One_Drive),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_One_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_One_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_One_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Saturator_One_Toggle),1}, params.at(names::Saturator_One_Toggle), true));

    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Two_Drive),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Two_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Two_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Two_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Saturator_Two_Toggle),1}, params.at(names::Saturator_Two_Toggle), true));

    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Three_Drive),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Three_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Three_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Three_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(juce::ParameterID{params.at(names::Saturator_Three_Toggle),1}, params.at(names::Saturator_Three_Toggle), true));

    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Single_Drive),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Single_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Single_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Saturator_Single_Mix),1}, "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Saturator_Single_Toggle),1}, params.at(names::Saturator_Single_Toggle), true));

    //wavefolder controls
    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Wavefolder_One_Type),1}, "Type", 0, 1, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_One_Drive_Sin),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::WaveFolder_One_Drive_Tri),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_One_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_One_Mix),1}, "Mix", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_One_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Wavefolder_One_Toggle),1}, params.at(names::Wavefolder_One_Toggle), false));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Wavefolder_Two_Type),1}, "Type", 0, 1, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Two_Drive_Sin),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::WaveFolder_Two_Drive_Tri),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Two_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Two_Mix),1}, "Mix", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Two_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Wavefolder_Two_Toggle),1}, params.at(names::Wavefolder_Two_Toggle), false));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Wavefolder_Three_Type),1}, "Type", 0, 1, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Three_Drive_Sin),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::WaveFolder_Three_Drive_Tri),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Three_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Three_Mix),1}, "Mix", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Three_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Wavefolder_Three_Toggle),1}, params.at(names::Wavefolder_Three_Toggle), false));

    layout.add(std::make_unique<AudioParameterInt>(juce::ParameterID{params.at(names::Wavefolder_Single_Type),1}, "Type", 0, 1, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Single_Drive_Sin),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::WaveFolder_Single_Drive_Tri),1}, "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Single_In),1}, "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Single_Mix),1}, "Mix", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(names::Wavefolder_Single_Out),1}, "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(names::Wavefolder_Single_Toggle),1}, params.at(names::Wavefolder_Single_Toggle), false));

    return layout;
}

void Mangl3rAudioProcessor::setToolbarOrder(std::vector<int> high, std::vector<int> mid, std::vector<int> low, std::vector<int> single)
{
    toolbarHighOrder = high;
    toolbarMidOrder = mid;
    toolbarLowOrder = low;
    toolbarSingleOrder = single;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Mangl3rAudioProcessor();
}
