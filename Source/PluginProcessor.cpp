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

    engine1.engineToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Engine_One_Toggle)));
    engine2.engineToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Engine_Two_Toggle)));
    engine3.engineToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Engine_Three_Toggle)));

    engine1.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_In)));
    engine1.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_Mix)));
    engine1.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_Out)));
    engine1.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Clipper_One_Type)));
    engine1.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_Threshold)));
    engine1.clipperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Clipper_One_Toggle)));

    engine1.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_One_Depth)));
    engine1.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_One_Rate)));
    engine1.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_One_In)));
    engine1.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_One_Mix)));
    engine1.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_One_Out)));
    engine1.crusherToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Bitcrusher_One_Toggle)));

    engine1.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_Drive)));
    engine1.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_In)));
    engine1.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_Mix)));
    engine1.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_Out)));
    engine1.satToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Saturator_One_Toggle)));

    engine1.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Waveshaper_One_Type)));
    engine1.waveShaperFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_Sin)));
    engine1.waveShaperFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_Quad)));
    engine1.waveShaperFactorsHolder[2] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_Factor)));
    engine1.waveShaperFactorsHolder[3] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_GB)));
    engine1.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_In)));
    engine1.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Mix)));
    engine1.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Out)));
    engine1.waveShaperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Waveshaper_One_Toggle)));

    engine1.wavefolderSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Wavefolder_One_Type)));
    engine1.wavefolderFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_One_Drive_Sin)));
    engine1.wavefolderFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::WaveFolder_One_Drive_Tri)));
    engine1.wavefolderInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_One_In)));
    engine1.wavefolderMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_One_Mix)));
    engine1.wavefolderOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_One_Out)));
    engine1.wavefolderToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Wavefolder_One_Toggle)));

    engine2.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_In)));
    engine2.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_Mix)));
    engine2.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_Out)));
    engine2.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Clipper_Two_Type)));
    engine2.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_Threshold)));
    engine2.clipperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Clipper_Two_Toggle)));

    engine2.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Depth)));
    engine2.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Rate)));
    engine2.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Two_In)));
    engine2.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Mix)));
    engine2.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Out)));
    engine2.crusherToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Toggle)));

    engine2.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_Drive)));
    engine2.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_In)));
    engine2.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_Mix)));
    engine2.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_Out)));
    engine2.satToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Saturator_Two_Toggle)));

    engine2.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Waveshaper_Two_Type)));
    engine2.waveShaperFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_Sin)));
    engine2.waveShaperFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_Quad)));
    engine2.waveShaperFactorsHolder[2] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_Factor)));
    engine2.waveShaperFactorsHolder[3] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_GB)));
    engine2.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_In)));
    engine2.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Mix)));
    engine2.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Out)));
    engine2.waveShaperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Waveshaper_Two_Toggle)));

    engine2.wavefolderSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Wavefolder_Two_Type)));
    engine2.wavefolderFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Two_Drive_Sin)));
    engine2.wavefolderFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::WaveFolder_Two_Drive_Tri)));
    engine2.wavefolderInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Two_In)));
    engine2.wavefolderMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Two_Mix)));
    engine2.wavefolderOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Two_Out)));
    engine2.wavefolderToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Wavefolder_Two_Toggle)));

    engine3.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_In)));
    engine3.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_Mix)));
    engine3.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_Out)));
    engine3.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Clipper_Three_Type)));
    engine3.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_Threshold)));
    engine3.clipperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Clipper_Three_Toggle)));

    engine3.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Depth)));
    engine3.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Rate)));
    engine3.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Three_In)));
    engine3.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Mix)));
    engine3.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Out)));
    engine3.crusherToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Toggle)));

    engine3.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_Drive)));
    engine3.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_In)));
    engine3.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_Mix)));
    engine3.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_Out)));
    engine3.satToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Saturator_Three_Toggle)));

    engine3.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Waveshaper_Three_Type)));
    engine3.waveShaperFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_Sin)));
    engine3.waveShaperFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_Quad)));
    engine3.waveShaperFactorsHolder[2] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_Factor)));
    engine3.waveShaperFactorsHolder[3] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_GB)));
    engine3.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_In)));
    engine3.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Mix)));
    engine3.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Out)));
    engine3.waveShaperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Waveshaper_Three_Toggle)));

    engine3.wavefolderSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Wavefolder_Three_Type)));
    engine3.wavefolderFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Three_Drive_Sin)));
    engine3.wavefolderFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::WaveFolder_Three_Drive_Tri)));
    engine3.wavefolderInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Three_In)));
    engine3.wavefolderMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Three_Mix)));
    engine3.wavefolderOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Wavefolder_Three_Out)));
    engine3.wavefolderToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(names::Wavefolder_Three_Toggle)));

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

    engine1.prepareToPlay(spec);
    engine2.prepareToPlay(spec);
    engine3.prepareToPlay(spec);

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

    osc.initialise([](float x) {return std::sin(x); });
    osc.prepare(spec);
    osc.setFrequency(200);

    gain.prepare(spec);
    gain.setGainDecibels(-12.f);
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

    if (globalBypass->get()) { return; }

    juce::AudioBuffer<float> copyBuffer;
    copyBuffer = buffer;
    auto copyBlock = juce::dsp::AudioBlock<float>(buffer);
    auto copyContext = juce::dsp::ProcessContextReplacing<float>(copyBlock);

    masterIn.setGainDecibels(masterInValue->get());
    masterIn.process(copyContext);

    levelMeterData.process(true, 0, buffer);
    levelMeterData.process(true, 1, buffer);

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
        engine1.process(ov2Block, toolbarHighOrder, ch);
        engine2.process(ov1Block, toolbarMidOrder, ch); 
        engine3.process(ov0Block, toolbarLowOrder, ch);
    }

    overSamplers[ovRate].processSamplesDown(fb0Ctx.getOutputBlock());
    overSamplers[ovRate + 4].processSamplesDown(fb1Ctx.getOutputBlock());
    overSamplers[ovRate + 8].processSamplesDown(fb2Ctx.getOutputBlock());

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    buffer.clear();

    auto addFilterBand = [nc = getTotalNumOutputChannels(), ns = numSamples](auto& inputBuffer, const auto& source)
        {
            for (auto i = 0; i < nc; i++)
            {
                inputBuffer.addFrom(i, 0, source, i, 0, ns);
            };
        };

    if(!lowMute->get())
        addFilterBand(buffer, filterBuffers[0]);
    if(!midMute->get())
        addFilterBand(buffer, filterBuffers[1]);
    if (!highMute->get())
        addFilterBand(buffer, filterBuffers[2]);

    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        auto data = buffer.getWritePointer(ch);
        auto dry = copyBuffer.getReadPointer(ch);

        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            data[s] = (data[s] * masterMix->get() / 100) + (dry[s] * (1 - masterMix->get() / 100));
        }
    }

    auto finalBlock = juce::dsp::AudioBlock<float>(buffer);
    auto finalContext = juce::dsp::ProcessContextReplacing<float>(finalBlock);

    masterOut.setGainDecibels(masterOutValue->get());
    masterOut.process(finalContext);

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

    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Global_Bypass), "Global Bypass", false));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Master_In_Gain), "In", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Master_Out_Gain), "Out", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Master_Mix), "Mix", mixRange, 100));

    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Select_Toolbar_One), "Toolbar One", true));
    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Select_Toolbar_Two), "Toolbar Two", false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Select_Toolbar_Three), "Toolbar Three", false));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Low_Mid_Crossover_Freq), "Low Xover", lowMidRange, 400));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Mid_High_Crossover_Freq), "High Xover", midHighRange, 2000));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Oversample_Rate), "OS Rate", 0, 3, 0));

    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Engine_One_Toggle), "Bypass", false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Engine_Two_Toggle), "Bypass", false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Engine_Three_Toggle), "Bypass", false));

    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Engine_One_Mute), "Mute", false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Engine_Two_Mute), "Mute", false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Engine_Three_Mute), "Mute", false));


    //Clipper Controls
    auto threshRange = NormalisableRange<float>(-60, 0, .1, 1);
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Clipper_One_Type), "Clipper Type", 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_One_Threshold), "Threshold", threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_One_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_One_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_One_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Clipper_One_Toggle), params.at(names::Clipper_One_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Clipper_Two_Type), "Clipper Type", 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Two_Threshold), "Threshold", threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Two_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Two_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Two_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Clipper_Two_Toggle), params.at(names::Clipper_Two_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Clipper_Three_Type), "Clipper Type", 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Three_Threshold), "Threshold", threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Three_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Three_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Three_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Clipper_Three_Toggle), params.at(names::Clipper_Three_Toggle), true));

    //WaveShaper Controls
    auto lessThanOne = NormalisableRange<float>(.01, .99, .01, 1);
    auto sineFactor = NormalisableRange<float>(.05, .95, .01, 1);
    auto moreThanOne = NormalisableRange<float>(1, 10, .01, 1);
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Waveshaper_One_Type), "Type", 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Drive_Sin), "Drive", sineFactor, .05));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Drive_Quad), "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Drive_Factor), "Drive", lessThanOne, .05));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Drive_GB), "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Waveshaper_One_Toggle), params.at(names::Waveshaper_One_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Waveshaper_Two_Type), "Type", 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Drive_Sin), "Drive", sineFactor, .05));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Drive_Quad), "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Drive_Factor), "Drive", lessThanOne, .05));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Drive_GB), "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Mix), "Dry/Wet", mixRange, 1000));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Waveshaper_Two_Toggle), params.at(names::Waveshaper_Two_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Waveshaper_Three_Type), "Type", 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Drive_Sin), "sin", sineFactor, .05));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Drive_Quad), "quad", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Drive_Factor), "factor", lessThanOne, .05));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Drive_GB), "GB", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Waveshaper_Three_Toggle), params.at(names::Waveshaper_Three_Toggle), true));

    //BitCrusher Controls
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_One_Depth), "Bit Depth", 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_One_Rate), "Bit Rate", 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_One_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_One_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_One_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Bitcrusher_One_Toggle), params.at(names::Bitcrusher_One_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_Two_Depth), "Bit Depth", 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_Two_Rate), "Bit Rate", 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Two_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Two_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Two_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Bitcrusher_Two_Toggle), params.at(names::Bitcrusher_Two_Toggle), true));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_Three_Depth), "Bit Depth", 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_Three_Rate), "Bit Rate", 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Three_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Three_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Three_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Bitcrusher_Three_Toggle), params.at(names::Bitcrusher_Three_Toggle), true));

    //Saturation Controls
    auto driveRange = NormalisableRange<float>(1, 10, .1, 1);
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_One_Drive), "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_One_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_One_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_One_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Saturator_One_Toggle), params.at(names::Saturator_One_Toggle), true));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Two_Drive), "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Two_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Two_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Two_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Saturator_Two_Toggle), params.at(names::Saturator_Two_Toggle), true));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Three_Drive), "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Three_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Three_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Three_Mix), "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique <AudioParameterBool>(params.at(names::Saturator_Three_Toggle), params.at(names::Saturator_Three_Toggle), true));

    //wavefolder controls
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Wavefolder_One_Type), "Type", 0, 1, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_One_Drive_Sin), "Sin", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::WaveFolder_One_Drive_Tri), "Tri", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_One_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_One_Mix), "Mix", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_One_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Wavefolder_One_Toggle), params.at(names::Wavefolder_One_Toggle), false));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Wavefolder_Two_Type), "Type", 0, 1, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_Two_Drive_Sin), "Sin", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::WaveFolder_Two_Drive_Tri), "Tri", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_Two_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_Two_Mix), "Mix", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_Two_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Wavefolder_Two_Toggle), params.at(names::Wavefolder_Two_Toggle), false));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Wavefolder_Three_Type), "Type", 0, 1, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_Three_Drive_Sin), "Sin", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::WaveFolder_Three_Drive_Tri), "Tri", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_Three_In), "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_Three_Mix), "Mix", mixRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Wavefolder_Three_Out), "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterBool>(params.at(names::Wavefolder_Three_Toggle), params.at(names::Wavefolder_Three_Toggle), false));

    return layout;
}

void Mangl3rAudioProcessor::setToolbarOrder(std::vector<int> high, std::vector<int> mid, std::vector<int> low)
{
    toolbarHighOrder = high;
    toolbarMidOrder = mid;
    toolbarLowOrder = low;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Mangl3rAudioProcessor();
}
