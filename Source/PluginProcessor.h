/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "DSP/kLevelMeterData.h"
#include "DSP/DistorkEngine.h"
#include "Utility/Params.h"
#include "Utility/KiTiK_utilityViz.h"
#include "Utility/PresetManager.h"

//==============================================================================
/**
*/
class Mangl3rAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Mangl3rAudioProcessor();
    ~Mangl3rAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "parameters", createParameterLayout() };

    LevelMeterData levelMeterData, zeroDbMeterData;
    FFTData fftData;
    std::unique_ptr<PresetManager> presetManager;
    PresetManager& getPresetManger() { return *presetManager; }

    void setToolbarOrder(std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>);

private:

    std::array<juce::dsp::Oversampling<float>, 12> overSamplers
    { {
        {2, 0, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 3, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},

        {2, 0, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 3, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},

        {2, 0, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 3, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
    } };

    int lastOSValue{ 0 };

    juce::dsp::Gain<float> masterIn;
    juce::dsp::Gain<float> masterOut;

    juce::AudioParameterBool* singleBandMode{ nullptr };

    juce::AudioParameterBool* globalBypass{ nullptr };
    juce::AudioParameterFloat* masterInValue{ nullptr };
    juce::AudioParameterFloat* masterOutValue{ nullptr };
    juce::AudioParameterFloat* masterMix{ nullptr };

    juce::AudioParameterBool* selectToolbarOne{ nullptr };
    juce::AudioParameterBool* selectToolbarTwo{ nullptr };
    juce::AudioParameterBool* selectToolbarThree{ nullptr };
    juce::AudioParameterInt* oversampleRate{ nullptr };

    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    //     fc0  fc1
    Filter LP1, AP2,
           HP1, LP2,
           HP2;

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };

    DistorkEngine engineHigh, engineMid, engineLow, engineSingle;

    std::array <juce::AudioBuffer<float>, 3> filterBuffers;
    std::vector<int> toolbarHighOrder{ 1, 2, 3, 4 };
    std::vector<int> toolbarMidOrder{ 1, 2, 3, 4 };
    std::vector<int> toolbarLowOrder{ 1, 2, 3, 4 };
    std::vector<int> toolbarSingleOrder{ 1, 2, 3, 4 };

    juce::AudioParameterBool* lowMute{ nullptr };
    juce::AudioParameterBool* midMute{ nullptr };
    juce::AudioParameterBool* highMute{ nullptr };

    //TODO:
        //Spectrum Anylzer:
            //Get image rendered and sized
            //Set up slider for vertical crossovers DONE
            //Where do I put analyzer button?
            // Click on section to allow for change in toolbar
                //I could do on mouse click for this? This will have to be done less
            //Add solo, bypass, mute?
            // Add Bars to frquency cutoff
            // Make sure frequency analyzer is lined up.
        //Toolbar:
            //Figure out how to set up toolbar with buttons DONE
            //Set up three of them DONE
            //Need to be able to distinguish from each of them DONE
            // Add/Fix oversampling ->chowdsp has one that might be worth using. DONE
        //DSP
            //Need to create Distork Engine, and have three instances of them DONE
            //Seperate the bands DONE
            ///Maybe add some more functionality. 
        //GUI
            //Maybe make it less wide?

    //BETA Feedback:
    // Fix bug on wavefoldr where numbers aren't switching -> Check waveshaper DONE
    // Check out bitcrusher DONE
    // Add single band mode DONE
    // Fix glitch with wavefoldr and shaper on hover DONE
    //add signalsmith algo. 

    /*juce::dsp::Oscillator<float> osc; //FFT graph testing
    juce::dsp::Gain<float> gain;*/

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Mangl3rAudioProcessor)
};
