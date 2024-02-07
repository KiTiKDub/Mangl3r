/*
  ==============================================================================

    Toolbar.h
    Created: 22 Jan 2024 10:04:29am
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "kLookAndFeel.h"
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
        auto bounds = getLocalBounds();

        auto toggle = bounds.reduced(bounds.getWidth() * .2, bounds.getHeight() * .2);
        toggle.setPosition(bounds.getPosition());
        toggle.translate(-20, bounds.getHeight() * .25);

        power.setBounds(toggle);
    }

private:
    juce::ToggleButton power;
    //juce::AudioProcessorValueTreeState::ButtonAttachment powerAT;
    Laf lnf;
};

struct ToolbarFactory : public TestToolbarItemFactory
{
    ToolbarFactory() {}

    enum ItemIds
    {
        saturation = 1,
        clipper = 2,
        waveshaper = 3,
        bitcrusher = 4
    };

    void getAllToolbarItemIds(juce::Array<int>& ids) override
    {
        ids.add(saturation);
        ids.add(clipper);
        ids.add(waveshaper);
        ids.add(bitcrusher);
    }

    void getDefaultItemSet(juce::Array<int>& ids) override
    {
        ids.add(saturation);
        ids.add(clipper);
        ids.add(waveshaper);
        ids.add(bitcrusher);
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
        }

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
    KitikToolbar* getHigh() { return &toolbarHigh; };
    KitikToolbar* getMid() { return &toolbarMid; };
    KitikToolbar* getLow() { return &toolbarLow; };

private:
    
    using Attachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    KitikToolbar toolbarHigh, toolbarMid, toolbarLow;
    ToolbarFactory tbf;
    Laf lnf;
    juce::ToggleButton low{ "Low" }, mid{ "Mids" }, high{ "Highs" };
    Attachment lowAT, midAT, highAT;

    std::unique_ptr<Attachment> satToggle1AT, satToggle2AT, satToggle3AT,
                                clipToggle1AT, clipToggle2AT, clipToggle3AT,
                                waveToggle1AT, waveToggle2AT, waveToggle3AT,
                                bitToggle1AT, bitToggle2AT, bitToggle3AT;

    std::vector<std::unique_ptr<Attachment>*> vectorAT{&satToggle1AT, &satToggle2AT, &satToggle3AT,
                                                       &clipToggle1AT, &clipToggle2AT, &clipToggle3AT,
                                                       &waveToggle1AT, &waveToggle2AT, &waveToggle3AT,
                                                       &bitToggle1AT, &bitToggle2AT, &bitToggle3AT};
};

