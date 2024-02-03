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
    using namespace Params;
    const auto& params = getParams();

    globalBypass = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("globalBypass"));
    masterInValue = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Master_In_Gain)));
    masterMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Master_Mix)));
    masterOutValue = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Master_Out_Gain)));

    selectToolbarOne = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectToolbarOne"));
    selectToolbarTwo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectToolbarTwo"));
    selectToolbarThree = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectToolbarThree"));

    engine1.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_In)));
    engine1.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_Mix)));
    engine1.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_Out)));
    engine1.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Clipper_One_Type)));
    engine1.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_One_Threshold)));

    engine1.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_One_Depth)));
    engine1.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_One_Rate)));
    engine1.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_One_In)));
    engine1.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_One_Mix)));
    engine1.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_One_Out)));

    engine1.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_Drive)));
    engine1.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_In)));
    engine1.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_Mix)));
    engine1.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_One_Out)));

    engine1.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Waveshaper_One_Type)));
    engine1.waveShaperSin = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_Sin)));
    engine1.waveShaperQuadratic = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_Quad)));
    engine1.waveShaperFactor = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_Quad)));
    engine1.waveShaperGB = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Drive_GB)));
    engine1.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_In)));
    engine1.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Mix)));
    engine1.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_One_Out)));

    engine2.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_In)));
    engine2.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_Mix)));
    engine2.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_Out)));
    engine2.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Clipper_Two_Type)));
    engine2.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Two_Threshold)));

    engine2.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Depth)));
    engine2.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Rate)));
    engine2.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Two_In)));
    engine2.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Mix)));
    engine2.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Two_Out)));

    engine2.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_Drive)));
    engine2.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_In)));
    engine2.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_Mix)));
    engine2.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Two_Out)));

    engine2.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Waveshaper_Two_Type)));
    engine2.waveShaperSin = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_Sin)));
    engine2.waveShaperQuadratic = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_Quad)));
    engine2.waveShaperFactor = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_Quad)));
    engine2.waveShaperGB = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Drive_GB)));
    engine2.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_In)));
    engine2.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Mix)));
    engine2.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Two_Out)));

    engine3.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_In)));
    engine3.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_Mix)));
    engine3.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_Out)));
    engine3.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Clipper_Three_Type)));
    engine3.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Clipper_Three_Threshold)));

    engine3.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Depth)));
    engine3.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Rate)));
    engine3.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Three_In)));
    engine3.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Mix)));
    engine3.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Bitcrusher_Three_Out)));

    engine3.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_Drive)));
    engine3.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_In)));
    engine3.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_Mix)));
    engine3.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Saturator_Three_Out)));

    engine3.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(names::Waveshaper_Three_Type)));
    engine3.waveShaperSin = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_Sin)));
    engine3.waveShaperQuadratic = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_Quad)));
    engine3.waveShaperFactor = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_Quad)));
    engine3.waveShaperGB = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Drive_GB)));
    engine3.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_In)));
    engine3.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Mix)));
    engine3.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(names::Waveshaper_Three_Out)));

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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Mangl3rAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout Mangl3rAudioProcessor::createParameterLayout()
{
    using namespace juce;
    using namespace Params;
    const auto& params = getParams();

    AudioProcessorValueTreeState::ParameterLayout layout;

    auto gainRange = NormalisableRange<float>(-24, 24, .1, 1);
    auto mixRange = NormalisableRange<float>(0, 100, 1, 1);

    layout.add(std::make_unique<AudioParameterBool>("globalBypass", "Global Bypass", false));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Master_In_Gain), params.at(names::Master_Mix), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Master_Out_Gain), params.at(names::Master_Out_Gain), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Master_Mix), params.at(names::Master_Mix), mixRange, 100));

    layout.add(std::make_unique<AudioParameterBool>("selectToolbarOne", "Toolbar One", true));
    layout.add(std::make_unique<AudioParameterBool>("selectToolbarTwo", "Toolbar Two", false));
    layout.add(std::make_unique<AudioParameterBool>("selectToolbarThree", "Toolbar Three", false));

    //Clipper Controls
    auto threshRange = NormalisableRange<float>(-60, 0, .1, 1);
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Clipper_One_Type), params.at(names::Clipper_One_Type), 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_One_Threshold), params.at(names::Clipper_One_Threshold), threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_One_In), params.at(names::Clipper_One_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_One_Out), params.at(names::Clipper_One_Out), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_One_Mix), params.at(names::Clipper_One_Mix), mixRange, 100));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Clipper_Two_Type), params.at(names::Clipper_Two_Type), 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Two_Threshold), params.at(names::Clipper_Two_Threshold), threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Two_In), params.at(names::Clipper_Two_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Two_Out), params.at(names::Clipper_Two_Out), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Two_Mix), params.at(names::Clipper_Two_Mix), mixRange, 100));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Clipper_Three_Type), params.at(names::Clipper_Three_Type), 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Three_Threshold), params.at(names::Clipper_Three_Threshold), threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Three_In), params.at(names::Clipper_Three_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Three_Out), params.at(names::Clipper_Three_Out), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Clipper_Three_Mix), params.at(names::Clipper_Three_Mix), mixRange, 100));

    //WaveShaper Controls
    auto lessThanOne = NormalisableRange<float>(.01, .99, .01, 1);
    auto sineFactor = NormalisableRange<float>(.05, .95, .01, 1);
    auto moreThanOne = NormalisableRange<float>(1, 10, .01, 1);
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Waveshaper_One_Type), params.at(names::Waveshaper_One_Type), 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Drive_Sin), params.at(names::Waveshaper_One_Drive_Sin), sineFactor, .5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Drive_Quad), params.at(names::Waveshaper_One_Drive_Quad), moreThanOne, 5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Drive_Factor), params.at(names::Waveshaper_One_Drive_Factor), lessThanOne, .5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Drive_GB), params.at(names::Waveshaper_One_Drive_GB), moreThanOne, 5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_In), params.at(names::Waveshaper_One_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Mix), params.at(names::Waveshaper_One_Mix), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_One_Out), params.at(names::Waveshaper_One_Out), mixRange, 100));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Waveshaper_Two_Type), params.at(names::Waveshaper_Two_Type), 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Drive_Sin), params.at(names::Waveshaper_Two_Drive_Sin), sineFactor, .5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Drive_Quad), params.at(names::Waveshaper_Two_Drive_Quad), moreThanOne, 5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Drive_Factor), params.at(names::Waveshaper_Two_Drive_Factor), lessThanOne, .5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Drive_GB), params.at(names::Waveshaper_Two_Drive_GB), moreThanOne, 5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_In), params.at(names::Waveshaper_Two_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Mix), params.at(names::Waveshaper_Two_Mix), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Two_Out), params.at(names::Waveshaper_Two_Out), mixRange, 100));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Waveshaper_Three_Type), params.at(names::Waveshaper_Three_Type), 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Drive_Sin), params.at(names::Waveshaper_Three_Drive_Sin), sineFactor, .5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Drive_Quad), params.at(names::Waveshaper_Three_Drive_Quad), moreThanOne, 5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Drive_Factor), params.at(names::Waveshaper_Three_Drive_Factor), lessThanOne, .5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Drive_GB), params.at(names::Waveshaper_Three_Drive_GB), moreThanOne, 5));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_In), params.at(names::Waveshaper_Three_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Mix), params.at(names::Waveshaper_Three_Mix), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Waveshaper_Three_Out), params.at(names::Waveshaper_Three_Out), mixRange, 100));

    //BitCrusher Controls
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_One_Depth), params.at(names::Bitcrusher_One_Depth), 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_One_Rate), params.at(names::Bitcrusher_One_Rate), 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_One_In), params.at(names::Bitcrusher_One_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_One_Out), params.at(names::Bitcrusher_One_Out), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_One_Mix), params.at(names::Bitcrusher_One_Mix), mixRange, 100));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_Two_Depth), params.at(names::Bitcrusher_Two_Depth), 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_Two_Rate), params.at(names::Bitcrusher_Two_Rate), 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Two_In), params.at(names::Bitcrusher_Two_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Two_Out), params.at(names::Bitcrusher_Two_Out), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Two_Mix), params.at(names::Bitcrusher_Two_Mix), mixRange, 100));

    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_Three_Depth), params.at(names::Bitcrusher_Three_Depth), 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>(params.at(names::Bitcrusher_Three_Rate), params.at(names::Bitcrusher_Three_Rate), 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Three_In), params.at(names::Bitcrusher_Three_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Three_Out), params.at(names::Bitcrusher_Three_Out), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Bitcrusher_Three_Mix), params.at(names::Bitcrusher_Three_Mix), mixRange, 100));

    //Saturation Controls
    auto driveRange = NormalisableRange<float>(1, 10, .1, 1);
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_One_Drive), params.at(names::Saturator_One_Drive), driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_One_In), params.at(names::Saturator_One_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_One_Out), params.at(names::Saturator_One_Out), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_One_Mix), params.at(names::Saturator_One_Mix), mixRange, 100));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Two_Drive), params.at(names::Saturator_Two_Drive), driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Two_In), params.at(names::Saturator_Two_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Two_Out), params.at(names::Saturator_Two_Out), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Two_Mix), params.at(names::Saturator_Two_Mix), mixRange, 100));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Three_Drive), params.at(names::Saturator_Three_Drive), driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Three_In), params.at(names::Saturator_Three_In), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Three_Out), params.at(names::Saturator_Three_Out), gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(names::Saturator_Three_Mix), params.at(names::Saturator_Three_Mix), mixRange, 100));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Mangl3rAudioProcessor();
}
