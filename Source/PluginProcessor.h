/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/kLevelMeterData.h"
#include "DSP/DistorkEngine.h"
#include "Utility/Params.h"
#include "Utility/KiTiK_utilityViz.h"

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

    LevelMeterData levelMeterData;
    FFTData fftData;

private:

    juce::AudioParameterBool* globalBypass{ nullptr };
    juce::AudioParameterFloat* masterInValue{ nullptr };
    juce::AudioParameterFloat* masterOutValue{ nullptr };
    juce::AudioParameterFloat* masterMix{ nullptr };

    juce::AudioParameterBool* selectToolbarOne{ nullptr };
    juce::AudioParameterBool* selectToolbarTwo{ nullptr };
    juce::AudioParameterBool* selectToolbarThree{ nullptr };

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };

    DistorkEngine engine1, engine2, engine3;

    //TODO:
        //Spectrum Anylzer:
            //Get image rendered and sized
            //Set up slider for vertical crossovers
            //Where do I put analyzer button?
            // Click on section to allow for change in toolbar
        //Toolbar:
            //Figure out how to set up toolbar with buttons
            //Set up three of them
            //Need to be able to distinguish from each of them
        //DSP
            //Need to create Distork Engine, and have three instances of them
            ///Maybe add some more functionality. 
        //GUI
            //Maybe make it less wide?

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Mangl3rAudioProcessor)
};
