/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Mangl3rAudioProcessorEditor::Mangl3rAudioProcessorEditor (Mangl3rAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&lnf);

    addAndMakeVisible(saturation);
    addChildComponent(clipper);
    addChildComponent(waveShaper);
    addChildComponent(bitCrusher);

    addAndMakeVisible(masterComp);
    addAndMakeVisible(toolbar);
    addAndMakeVisible(analyzer);

    setSize (700, 600);
    startTimerHz(24);
}

Mangl3rAudioProcessorEditor::~Mangl3rAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void Mangl3rAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    auto bounds = getLocalBounds();

    auto analyzerBounds = bounds.removeFromTop(200);
    g.drawRect(analyzerBounds);
    g.drawFittedText("Analyzer", analyzerBounds, juce::Justification::centred, 1);

    auto selectArea = bounds.removeFromBottom(bounds.getHeight() * .15);
    g.drawRect(selectArea);
    g.drawFittedText("Toolbar", selectArea, juce::Justification::centred, 1);

    auto masterArea = bounds.removeFromRight(bounds.getWidth() * .3);
    g.drawRect(masterArea);
    g.drawFittedText("Master", masterArea, juce::Justification::centred, 1);

    g.drawRect(bounds);
    g.drawFittedText("Processing", bounds, juce::Justification::centred, 1);
}

void Mangl3rAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto analyzerBounds = bounds.removeFromTop(200);
    auto selectArea = bounds.removeFromBottom(bounds.getHeight() * .15);
    auto masterArea = bounds.removeFromRight(bounds.getWidth() * .3);

    saturation.setBounds(bounds);
    clipper.setBounds(bounds);
    waveShaper.setBounds(bounds);
    bitCrusher.setBounds(bounds);

    masterComp.setBounds(masterArea);
    toolbar.setBounds(selectArea);

    analyzer.setBounds(analyzerBounds);
}

void Mangl3rAudioProcessorEditor::timerCallback()
{
    if(toolbar.isMouseButtonDown(true))
        displayCorrectDistortion();

    auto currentToolbar = toolbar.getCurrentEffect();
    auto id = currentToolbar->getAllItems();

    auto toolbarHigh = toolbar.getHigh();
    auto toolbarMid = toolbar.getMid();
    auto toolbarLow = toolbar.getLow();

    audioProcessor.setToolbarOrder(toolbarHigh->getAllItems(), toolbarMid->getAllItems(), toolbarLow->getAllItems());
}

void Mangl3rAudioProcessorEditor::displayCorrectDistortion()
{
    auto currentToolbar = toolbar.getCurrentEffect();

    auto& childComps = currentToolbar->getChildren();
    for (auto* child : childComps)
    {
        if (auto* tbComp = dynamic_cast<KitikToolbarItemComponent*>(child))
        {
            if (tbComp->isClicked)
            {
                auto name = tbComp->getName();

                if (name == "Saturation")
                {
                    saturation.setVisible(true);
                    clipper.setVisible(false);
                    waveShaper.setVisible(false);
                    bitCrusher.setVisible(false);

                    saturation.updateAttachments(audioProcessor.apvts, toolbar);

                }
                else if (name == "Clipper")
                {
                    saturation.setVisible(false);
                    clipper.setVisible(true);
                    waveShaper.setVisible(false);
                    bitCrusher.setVisible(false);

                    clipper.updateAttachments(audioProcessor.apvts, toolbar);

                }
                else if (name == "Waveshaper")
                {
                    saturation.setVisible(false);
                    clipper.setVisible(false);
                    waveShaper.setVisible(true);
                    bitCrusher.setVisible(false);

                    waveShaper.updateAttachments(audioProcessor.apvts, toolbar);
                }
                else if (name == "Bitcrusher")
                {
                    saturation.setVisible(false);
                    clipper.setVisible(false);
                    waveShaper.setVisible(false);
                    bitCrusher.setVisible(true);

                    bitCrusher.updateAttachments(audioProcessor.apvts, toolbar);
                }

            }
        }
    }
}
