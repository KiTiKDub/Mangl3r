/*
  ==============================================================================

    PresetPanel.h
    Created: 11 Jan 2024 6:09:12pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_core/juce_core.h"
#include "PresetManager.h"
#include "../GUI/kLookAndFeel.h"

struct PresetPanel : public juce::Component
{
    PresetPanel(PresetManager& pm);
    virtual ~PresetPanel() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:

    Laf lnf;

    PresetManager& presetManager;
    juce::PopupMenu popup;

    void configureButton(juce::TextButton& button, const juce::String& buttonText);
    void loadPresetList();

    juce::TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
    juce::ComboBox presetList;

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)

};