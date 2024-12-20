/*
  ==============================================================================

    MasterControls.h
    Created: 17 Nov 2023 4:39:37pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"
#include "../PluginProcessor.h"
#include "kLevelMeter.h"
#include "SliderWithLabels.h"
#include "../Utility/Params.h"

struct GainCompensator : public juce::Component
{
  GainCompensator(Mangl3rAudioProcessor &);
  ~GainCompensator() {};

  void paint(juce::Graphics &g) override;
  void resized() override {};
  float getCompensateValue();

private:
  Mangl3rAudioProcessor &audioP;
  juce::SmoothedValue<float> smoothValue;
};

struct MasterComp : public juce::Component, juce::Timer
{
    MasterComp(Mangl3rAudioProcessor&);
    ~MasterComp();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    bool getSingleToggleState();

private:

    Mangl3rAudioProcessor& audioP;

    juce::Image logo;
    juce::Font newFont;

    juce::URL url{ "https://kwhaley5.gumroad.com/" };

    juce::HyperlinkButton gumroad{ "More Plugins", url };

    void updateSWL(juce::AudioProcessorValueTreeState& apvts);

    std::array<LevelMeter, 2> meter;
    std::array<LevelMeter, 2> outMeter;

    std::unique_ptr<SliderWithLabels> inGain, mix, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainAT, mixAT, outGainAT;

    juce::ToggleButton bypass{ "On" }, single{ "Single" };
    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAT, singleAT;

    GainCompensator gainCompensator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterComp)
};