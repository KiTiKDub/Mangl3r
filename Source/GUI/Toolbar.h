/*
  ==============================================================================

    Toolbar.h
    Created: 22 Jan 2024 10:04:29am
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "kLookAndFeel.h"
#include "rotarySliderWithLabels.h"
#include "../Utility/KitikToolbar.h"
#include "../PluginProcessor.h"
#include "../Utility/Params.h"


struct ToolbarItem : public KitikToolbarItemComponent
{

    ToolbarItem(const int toolbarItemId, const juce::String& name)
        : KitikToolbarItemComponent(toolbarItemId, name, true)
    {
        setLookAndFeel(&lnf);
        //lnf.setColour(0x1003220, juce::Colour(64u, 194u, 230u));
        lnf.setColour(0x1003230, juce::Colour(64u, 194u, 230u).withAlpha(.4f));
        addAndMakeVisible(power);
        power.setComponentID("Power");
        power.setName(name);
        setComponentID("tbItem");

    }
    ~ToolbarItem()
    {
        setLookAndFeel(nullptr);
    }

    bool getToolbarItemSizes(int /*toolbarDepth*/, bool isVertical,
        int& preferredSize, int& minSize, int& maxSize) override
    {
        if (isVertical)
            return false;

        preferredSize = 250;
        minSize = 80;
        maxSize = 300;
        return true;
    }

    void paintButtonArea(juce::Graphics& g, int, int, bool, bool) override
    {
        g.setColour(juce::Colours::white);
        g.drawFittedText(power.getName(), getLocalBounds().toNearestInt(), juce::Justification::centred, 1);
    }

    void contentAreaChanged(const juce::Rectangle<int>& newArea) override
    {
        juce::ignoreUnused();
        auto bounds = getLocalBounds();

        auto toggle = bounds.reduced(bounds.getWidth() * .25, bounds.getHeight() * .25);
        toggle.setPosition(bounds.getPosition());
        toggle.translate(-15, /*bounds.getHeight() * .25*/0);

        power.setBounds(toggle);
    }

private:
    juce::ToggleButton power;
    Laf lnf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolbarItem)
};

struct ToolbarFactory : public TestToolbarItemFactory
{
    ToolbarFactory() {}

    enum ItemIds
    {
        saturation = 1,
        clipper = 2,
        waveshaper = 3,
        bitcrusher = 4,
        wavefolder = 5
    };

    void getAllToolbarItemIds(juce::Array<int>& ids) override
    {
        ids.add(saturation);
        ids.add(clipper);
        ids.add(waveshaper);
        ids.add(bitcrusher);
        ids.add(wavefolder);
    }

    void getDefaultItemSet(juce::Array<int>& ids) override
    {
        ids.add(saturation);
        ids.add(clipper);
        ids.add(waveshaper);
        ids.add(bitcrusher);
        ids.add(wavefolder);
    }

    KitikToolbarItemComponent* createItem(int itemId) override
    {
        switch (itemId)
        {
        case saturation:
            return new ToolbarItem(saturation, "Saturation");
        case clipper:
            return new ToolbarItem(clipper, "Clipper");
        case waveshaper:
            return new ToolbarItem(waveshaper, "Waveshaper");
        case bitcrusher:
            return new ToolbarItem(bitcrusher, "Bitcrusher");
        case wavefolder:
            return new ToolbarItem(wavefolder, "Wavefolder");
        }
        
        return nullptr;
    }
};


struct ToolbarComp : public juce::Component
{
    ToolbarComp(juce::AudioProcessorValueTreeState&);
    ~ToolbarComp();

    void resized() override;

    KitikToolbar* getCurrentEffect();
    juce::String getActiveBand();
    void setPowerButtons(juce::AudioProcessorValueTreeState&);
    KitikToolbar* getHigh() { return &toolbarHigh; }
    KitikToolbar* getMid() { return &toolbarMid; }
    KitikToolbar* getLow() { return &toolbarLow; }
    KitikToolbar* getSingle() { return &toolbarSingle; }
    void updateSingleToggleState(bool);

private:

    void attachRSWL(juce::AudioProcessorValueTreeState& apvts);
    
    using Attachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    KitikToolbar toolbarHigh, toolbarMid, toolbarLow, toolbarSingle;
    ToolbarFactory tbf;
    Laf lnf;
    juce::ToggleButton low{ "Low" }, mid{ "Mid" }, high{ "High" };
    Attachment lowAT, midAT, highAT;

    std::unique_ptr<Attachment> satToggleHighAT, satToggleMidAT, satToggleLowAT, satToggleSingleAT,
                                clipToggleHighAT, clipToggleMidAT, clipToggleLowAT, clipToggleSingleAT,
                                waveToggleHighAT, waveToggleMidAT, waveToggleLowAT, waveToggleSingleAT,
                                bitToggleHighAT, bitToggleMidAT, bitToggleLowAT, bitToggleSingleAT,
                                folderToggleHighAT, folderToggleMidAT, folderToggleLowAT, folderToggleSingleAT;

    std::vector<std::unique_ptr<Attachment>*> vectorAT{&satToggleHighAT, &satToggleMidAT, &satToggleLowAT, &satToggleSingleAT,
                                                       &clipToggleHighAT, &clipToggleMidAT, &clipToggleLowAT, &clipToggleSingleAT,
                                                       &waveToggleHighAT, &waveToggleMidAT, &waveToggleLowAT, &waveToggleSingleAT,
                                                       &bitToggleHighAT, &bitToggleMidAT, &bitToggleLowAT, &bitToggleSingleAT,
                                                       &folderToggleHighAT, &folderToggleMidAT, &folderToggleLowAT, &folderToggleSingleAT};

    std::unique_ptr<RotarySliderWithLabels> oversampleSelect;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oversampleSelectAT;

    std::vector<juce::String> oversamplingText{ "1x", "2x", "4x", "8x" };
    const std::map<Params::names, juce::String>& params = Params::getParams();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolbarComp)
};

