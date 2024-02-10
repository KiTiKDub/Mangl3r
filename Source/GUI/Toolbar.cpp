/*
  ==============================================================================

    Toolbar.cpp
    Created: 22 Jan 2024 10:04:29am
    Author:  kylew

  ==============================================================================
*/

#include "Toolbar.h"

ToolbarComp::ToolbarComp(AudioProcessorValueTreeState& apvts) :
    lowAT(apvts, "Toolbar Three", low),
    midAT(apvts, "Toolbar Two", mid),
    highAT(apvts, "Toolbar One", high)
{
    setLookAndFeel(&lnf);

    attachRSWL(apvts);

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

    addAndMakeVisible(*oversampleSelect);

    setPowerButtons(apvts);
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

    oversampleSelect->setBounds(overSelect);
}

KitikToolbar* ToolbarComp::getCurrentEffect()
{
    if (high.getToggleState())
    {
        return &toolbarHigh;

    }
    else if (mid.getToggleState())
    {
        return &toolbarMid;
    }
    else if(low.getToggleState())
    {
        return &toolbarLow;
    }
}

juce::String ToolbarComp::getActiveBand()
{
    if (high.getToggleState())
    {
        return toolbarHigh.getComponentID();

    }
    else if (mid.getToggleState())
    {
        return toolbarMid.getComponentID();
    }
    else if(low.getToggleState())
    {
        return toolbarLow.getComponentID();
    }
}

void ToolbarComp::setPowerButtons(juce::AudioProcessorValueTreeState& apvts)
{
    auto children = toolbarHigh.getChildren();
    auto childrenMid = toolbarMid.getChildren();
    auto childrenLow = toolbarLow.getChildren();

    //Needs to remove "Additional Items" from the array
    children.removeLast(1);
    childrenMid.removeLast(1);
    childrenLow.removeLast(1);

    children.addArray(childrenMid);
    children.addArray(childrenLow);

    auto highOrder = toolbarHigh.getAllItems();
    auto midOrder = toolbarMid.getAllItems();
    auto lowOrder = toolbarLow.getAllItems();

    const auto& params = Params::getParams();
    std::vector<Params::names> Names
    {
        Params::names::Saturator_One_Toggle,
        Params::names::Clipper_One_Toggle,
        Params::names::Waveshaper_One_Toggle,
        Params::names::Bitcrusher_One_Toggle,   

        Params::names::Saturator_Two_Toggle,
        Params::names::Clipper_Two_Toggle,
        Params::names::Waveshaper_Two_Toggle,
        Params::names::Bitcrusher_Two_Toggle,

        Params::names::Saturator_Three_Toggle,
        Params::names::Clipper_Three_Toggle,
        Params::names::Waveshaper_Three_Toggle,
        Params::names::Bitcrusher_Three_Toggle,
    };

    for (int i = 0; i < children.size(); i++) //maybe change this so it can dynmically update the power buttons
    {
        if (auto* tbComp = dynamic_cast<KitikToolbarItemComponent*>(children[i]))
        {
            if(auto* power = dynamic_cast<juce::ToggleButton*>(tbComp->getChildComponent(0)))
            {
                auto check = Names.at(i);
                auto ID = apvts.getParameter(params.at(Names.at(i)))->getParameterID();
                auto attachment = vectorAT.at(i);
                *attachment = std::make_unique<Attachment>(apvts, ID, *power);

                String toggleState;
                if (power->getToggleState())
                {
                    toggleState = "on";
                }
                else
                    toggleState = "off";

                DBG("Toggle State at Attachment: " << toggleState);
            }
            
        }
    }
}

void ToolbarComp::attachRSWL(juce::AudioProcessorValueTreeState& apvts)
{
    auto name = Params::names::Oversample_Rate;

    auto& osParam = getParam(apvts, params, name);
    oversampleSelect = std::make_unique<RotarySliderWithLabels>(&osParam, "", "OverSampling");
    makeAttachment(oversampleSelectAT, apvts, params, name, *oversampleSelect);
    addLabelPairs(oversampleSelect->labels, 1, 3, osParam, "", 14, oversamplingText);

    oversampleSelect.get()->onValueChange = [this, &osParam]()
        {
            addLabelPairs(oversampleSelect->labels, 1, 3, osParam, "", 14, oversamplingText);
        };
}
