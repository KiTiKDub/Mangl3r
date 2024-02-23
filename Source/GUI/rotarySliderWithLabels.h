/*
  ==============================================================================

    rotarySliderWithLabels.h
    Created: 6 Nov 2023 5:57:57pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct RotarySliderWithLabels : juce::Slider {
    RotarySliderWithLabels(juce::RangedAudioParameter* rap, const juce::String& unitSuffix, const juce::String title) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(rap),
        suffix(unitSuffix)
    {
        setName(title);
    }

    struct LabelPos {
        int pos;
        juce::String label;
        int fontSize;

    };

    juce::Array<LabelPos> labels;

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;

    void changeParam(juce::RangedAudioParameter* p);

protected:
    juce::RangedAudioParameter* param;
    juce::String suffix;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotarySliderWithLabels)
};

template <
    typename Attachment,
    typename APVTS,
    typename Params,
    typename ParamName,
    typename SliderType
>
void makeAttachment(std::unique_ptr<Attachment>& attachment,
    APVTS& apvts,
    const Params& params,
    const ParamName& name,
    SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts,
        params.at(name),
        slider);
}

template <
    typename APVTS,
    typename Params,
    typename Name
>
juce::RangedAudioParameter& getParam(APVTS& apvts, Params& params, const Name& name)
{
    auto param = apvts.getParameter(params.at(name));

    return *param;
}

template <
    typename Labels,
    typename ParamType,
    typename SuffixType
>
void addLabelPairs(Labels& labels, const int posOne, const int posTwo, const ParamType& param, const SuffixType& suffix, const int fontSize = 14, std::vector<juce::String> array = std::vector<juce::String>())
{
    labels.clear();
    labels.add({ posOne, getValString(param, true, suffix, array), fontSize });
    labels.add({ posTwo, getValString(param, false, suffix, array), fontSize });
}

juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix, std::vector<juce::String>);