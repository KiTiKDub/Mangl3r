/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Mangl3rAudioProcessorEditor::Mangl3rAudioProcessorEditor (Mangl3rAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), presetPanel(p.getPresetManger())
{
    setLookAndFeel(&lnf);

    addAndMakeVisible(saturation);
    addChildComponent(clipper);
    addChildComponent(waveShaper);
    addChildComponent(bitCrusher);
    addChildComponent(wavefolder);

    addAndMakeVisible(masterComp);
    addAndMakeVisible(toolbar);
    addAndMakeVisible(analyzer);

    addChildComponent(presetPanel);
    openPresetManager.setToggleState(false, false);
    addAndMakeVisible(openPresetManager);

    openPresetManager.setClickingTogglesState(true);

    openPresetManager.onClick = [this]()
        {
            if (openPresetManager.getToggleState())
            {
                presetPanel.setVisible(true);
                analyzer.setVisible(false);
            }
            else
            {
                presetPanel.setVisible(false);
                analyzer.setVisible(true);
            }
        };

    setSize (700, 600);
    startTimerHz(60);
}

Mangl3rAudioProcessorEditor::~Mangl3rAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void Mangl3rAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    auto bounds = getLocalBounds();

    auto analyzerBounds = bounds.removeFromTop(200);
    g.drawHorizontalLine(analyzerBounds.getBottom(), bounds.getX(), bounds.getRight());

    auto logoSpace = bounds.reduced(bounds.getWidth() * .425, bounds.getHeight() * .425);
    logoSpace.setX(bounds.getTopLeft().getX());
    logoSpace.setY(bounds.getTopLeft().getY());
    logoSpace.translate(-25, 0);

    auto logo = juce::ImageCache::getFromMemory(BinaryData::KITIK_LOGO_NO_BKGD_png, BinaryData::KITIK_LOGO_NO_BKGD_pngSize);

    auto selectArea = bounds.removeFromBottom(bounds.getHeight() * .15);
    g.drawHorizontalLine(selectArea.getY() - 1, bounds.getX(), bounds.getRight());

    auto masterArea = bounds.removeFromRight(bounds.getWidth() * .3);
    g.drawVerticalLine(masterArea.getX(), masterArea.getY(), masterArea.getBottom());

    openPresetManager.setImages(true, true, true, logo, 0, juce::Colours::white, juce::Image(), 0, juce::Colours::white, juce::Image(), 0, juce::Colour(64u, 194u, 230u));
    openPresetManager.setBounds(logoSpace);
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
    wavefolder.setBounds(bounds);

    masterComp.setBounds(masterArea);
    toolbar.setBounds(selectArea);

    analyzer.setBounds(analyzerBounds);
    presetPanel.setBounds(analyzerBounds);
}

void Mangl3rAudioProcessorEditor::timerCallback()
{
    if(toolbar.isMouseOver(true)) //only update when mouse is in vicinity, a little better
        displayCorrectDistortion();

    analyzer.update();

    auto currentToggleState = !masterComp.getSingleToggleState();

    if (lastToggleState != currentToggleState)
    {
        toolbar.updateSingleToggleState(currentToggleState);
        displayCorrectDistortion();
        lastToggleState = currentToggleState;
    }

    auto toolbarHigh = toolbar.getHigh();
    auto toolbarMid = toolbar.getMid();
    auto toolbarLow = toolbar.getLow();
    auto toolbarSingle = toolbar.getSingle();

    audioProcessor.setToolbarOrder(toolbarHigh->getAllItems(), toolbarMid->getAllItems(), toolbarLow->getAllItems(), toolbarSingle->getAllItems());
}

void Mangl3rAudioProcessorEditor::displayCorrectDistortion()
{
    auto currentToolbar = toolbar.getCurrentEffect(); //maybe I can make the band toggles buttons public and direct access them on click, then I can bring back the if condition. Or just do all of this in the on click
    //optimize this later.

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
                    wavefolder.setVisible(false);

                    saturation.updateAttachments(audioProcessor.apvts, toolbar);

                }
                else if (name == "Clipper")
                {
                    saturation.setVisible(false);
                    clipper.setVisible(true);
                    waveShaper.setVisible(false);
                    bitCrusher.setVisible(false);
                    wavefolder.setVisible(false);

                    clipper.updateAttachments(audioProcessor.apvts, toolbar);

                }
                else if (name == "Waveshaper")
                {
                    saturation.setVisible(false);
                    clipper.setVisible(false);
                    waveShaper.setVisible(true);
                    bitCrusher.setVisible(false);
                    wavefolder.setVisible(false);

                    waveShaper.updateAttachments(audioProcessor.apvts, toolbar);
                }
                else if (name == "Bitcrusher")
                {
                    saturation.setVisible(false);
                    clipper.setVisible(false);
                    waveShaper.setVisible(false);
                    bitCrusher.setVisible(true);
                    wavefolder.setVisible(false);

                    bitCrusher.updateAttachments(audioProcessor.apvts, toolbar);
                }
                else if (name == "Wavefolder")
                {
                    saturation.setVisible(false);
                    clipper.setVisible(false);
                    waveShaper.setVisible(false);
                    bitCrusher.setVisible(false);
                    wavefolder.setVisible(true);

                    wavefolder.updateAttachments(audioProcessor.apvts, toolbar);
                }

            }
        }
    }
}
