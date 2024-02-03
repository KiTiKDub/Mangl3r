/*
  ==============================================================================

    Toolbar.cpp
    Created: 22 Jan 2024 10:04:29am
    Author:  kylew

  ==============================================================================
*/

#include "Toolbar.h"

ToolbarComp::ToolbarComp(AudioProcessorValueTreeState& apvts) :
    lowAT(apvts, "selectToolbarThree", low), 
    midAT(apvts, "selectToolbarTwo", mid),
    highAT(apvts, "selectToolbarOne", high)
{
    setLookAndFeel(&lnf);

    addAndMakeVisible(toolbarHigh);
    toolbarHigh.addDefaultItems(tbf);
    toolbarHigh.setComponentID("High");
    toolbarHigh.setEditingActive(true);
    toolbarHigh.setName("High");

    addChildComponent(toolbarMid);
    toolbarMid.addDefaultItems(tbf);
    toolbarMid.setComponentID("Mid");
    toolbarMid.setEditingActive(true);

    addChildComponent(toolbarLow);
    toolbarLow.addDefaultItems(tbf);
    toolbarLow.setComponentID("Low");
    toolbarLow.setEditingActive(true);


    addAndMakeVisible(low);
    low.setComponentID("Select");
    addAndMakeVisible(mid);
    mid.setComponentID("Select");
    addAndMakeVisible(high);
    high.setComponentID("Select");

    high.setToggleState(true, juce::NotificationType::dontSendNotification);

    low.setRadioGroupId(1);
    mid.setRadioGroupId(1);
    high.setRadioGroupId(1);

    high.onClick = [this]()
        {
            toolbarHigh.setVisible(true);
            toolbarMid.setVisible(false);
            toolbarLow.setVisible(false);
        };
    mid.onClick = [this]()
        {
            toolbarHigh.setVisible(false);
            toolbarMid.setVisible(true);
            toolbarLow.setVisible(false);
        };
    low.onClick = [this]()
        {
            toolbarHigh.setVisible(false);
            toolbarMid.setVisible(false);
            toolbarLow.setVisible(true);
        };
}

ToolbarComp::~ToolbarComp()
{
    setLookAndFeel(nullptr);
}

void ToolbarComp::resized()
{
    auto bounds = getLocalBounds();
    auto barSelect = bounds.removeFromLeft(bounds.getWidth() * .1);
    auto overSelect = bounds.removeFromRight(bounds.getWidth() * .11);

    auto topSelect = barSelect.removeFromTop(barSelect.getHeight() * .33);
    auto midSelect = barSelect.removeFromTop(barSelect.getHeight() * .5);

    toolbarHigh.setBounds(bounds);
    toolbarMid.setBounds(bounds);
    toolbarLow.setBounds(bounds);

    high.setBounds(topSelect);
    mid.setBounds(midSelect);
    low.setBounds(barSelect);
}

KitikToolbar* ToolbarComp::getCurrentEffect()
{
    if (toolbarHigh.isVisible())
    {
        return &toolbarHigh;

    }
    else if (toolbarMid.isVisible())
    {
        return &toolbarMid;
    }
    else
    {
        return &toolbarLow;
    }
}

juce::String ToolbarComp::getActiveBand()
{
    if (toolbarHigh.isVisible())
    {
        return toolbarHigh.getComponentID();

    }
    else if (toolbarMid.isVisible())
    {
        return toolbarMid.getComponentID();
    }
    else
    {
        return toolbarLow.getComponentID();
    }
}
