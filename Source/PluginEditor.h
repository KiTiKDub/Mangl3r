/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/kLookAndFeel.h"
#include "GUI/MasterControls.h"
#include "GUI/Toolbar.h"
#include "GUI/Saturation.h"
#include "GUI/Clipper.h"
#include "GUI/WaveShaper.h"
#include "GUI/BitCrusher.h"
#include "GUI/Analyzer.h"
#include "GUI/Wavefolder.h"

//==============================================================================
/**
*/
class Mangl3rAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    Mangl3rAudioProcessorEditor(Mangl3rAudioProcessor&);
    ~Mangl3rAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
    void displayCorrectDistortion();

private:

    Laf lnf;

    Mangl3rAudioProcessor& audioProcessor;
    ToolbarComp toolbar{ audioProcessor.apvts };

    AnalyzerComp analyzer{ audioProcessor };

    SaturationComp saturation{ audioProcessor.apvts, toolbar };
    ClipperComp clipper{ audioProcessor.apvts, toolbar };
    WaveShaperComp waveShaper{ audioProcessor.apvts, toolbar };
    BitCrusherComp bitCrusher{ audioProcessor.apvts, toolbar };
    WavefolderComp wavefolder{ audioProcessor.apvts, toolbar };

    MasterComp masterComp{ audioProcessor };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Mangl3rAudioProcessorEditor)
};
