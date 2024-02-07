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

    addAndMakeVisible(masterComp);
    addAndMakeVisible(toolbar);
    addAndMakeVisible(saturationHigh);
    addChildComponent(SaturationMid);
    addChildComponent(bitcrusherHigh);
    addChildComponent(bitcrusherMid);
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

    saturationHigh.setBounds(bounds);
    SaturationMid.setBounds(bounds);
    bitcrusherHigh.setBounds(bounds);
    bitcrusherMid.setBounds(bounds);

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
                if (tbComp->getName() == "Saturation")
                {   
                    saturationHigh.setVisible(true);
                    bitcrusherHigh.setVisible(false);
                }
                else if(tbComp->getName() == "Bitcrusher")
                {
                    saturationHigh.setVisible(false);
                    bitcrusherHigh.setVisible(true);
                }
                
            }
        }
    }
}
