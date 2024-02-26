/*
  ==============================================================================

    PresetPanel.cpp
    Created: 11 Jan 2024 6:09:12pm
    Author:  kylew

  ==============================================================================
*/

#include "PresetPanel.h"

PresetPanel::PresetPanel(PresetManager& pm)
    : presetManager(pm)
{
    setLookAndFeel(&lnf);

    configureButton(saveButton, "Save");
    configureButton(deleteButton, "Delete");
    configureButton(previousPresetButton, "Previous");
    configureButton(nextPresetButton, "Next");

    presetList.setTextWhenNothingSelected("Init Preset");
    presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(presetList);
    loadPresetList();

    popup.addItem(1, "Are you sure you want to delete?", false);
    popup.addItem(2, "Yes");
    popup.addItem(3, "No");

    saveButton.onClick = [this]()
        {
            fileChooser = std::make_unique<juce::FileChooser>(
                "Please enter the name of the preset to save",
                PresetManager::defaultDirectory,
                "*." + PresetManager::extension
            );
            fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
                {
                    const auto resultFile = chooser.getResult();
                    presetManager.savePreset(resultFile.getFileNameWithoutExtension());
                    loadPresetList();
                });
        };

    deleteButton.onClick = [this]()
        {
            popup.showMenuAsync(juce::PopupMenu::Options(),
                [this](int result)
                {
                    if (result == 2)
                    {
                        presetManager.deletePreset(presetManager.getCurrentPreset());
                        loadPresetList();
                    }
                });
        };

    previousPresetButton.onClick = [this]()
        {
            const auto index = presetManager.loadPreviousPreset();
            presetList.setSelectedItemIndex(index, juce::dontSendNotification);
        };

    nextPresetButton.onClick = [this]()
        {
            const auto index = presetManager.loadNextPreset();
            presetList.setSelectedItemIndex(index, juce::dontSendNotification);
        };

    presetList.onChange = [this]()
        {
            presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));           
        };
}

PresetPanel::~PresetPanel()
{
    setLookAndFeel(nullptr);
}

void PresetPanel::resized()
{
    auto bounds = getLocalBounds();
    auto left = bounds.removeFromLeft(bounds.getWidth() * .25);
    auto right = bounds.removeFromRight(bounds.getWidth() * .33);
    auto tMiddle = bounds.removeFromTop(bounds.getHeight() * .2).reduced(5);

    auto tLeft = left.removeFromTop(left.getHeight() * .2).reduced(5);
    auto tRight = right.removeFromTop(right.getHeight() * .2).reduced(5);

    auto saveArea = left.removeFromBottom(left.getHeight() * .5).reduced(5);
    saveArea.removeFromBottom(saveArea.getHeight() * .5);

    auto deleteArea = right.removeFromBottom(right.getHeight() * .5).reduced(5);
    deleteArea.removeFromBottom(right.getHeight() * .5);

    presetList.setBounds(tMiddle);
    previousPresetButton.setBounds(tLeft);
    nextPresetButton.setBounds(tRight);
    saveButton.setBounds(saveArea);
    deleteButton.setBounds(deleteArea);
}

void PresetPanel::paint(juce::Graphics& g)
{
    g.setFont(5);
}

void PresetPanel::configureButton(juce::TextButton& button, const juce::String& buttonText)
{
    button.setButtonText(buttonText);
    button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(button);
}

void PresetPanel::loadPresetList()
{
    presetList.clear(juce::dontSendNotification);
    const auto allPresets = presetManager.getAllPresets();
    const auto currentPreset = presetManager.getCurrentPreset();
    presetList.addItemList(allPresets, 1);
    presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
}
