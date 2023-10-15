//
// Created by Kai on 15.10.23.
//

#include <juce_audio_processors/juce_audio_processors.h>

#ifndef AUDIO_PLUGIN_EXAMPLE_ROTARYSLIDER_H
#define AUDIO_PLUGIN_EXAMPLE_ROTARYSLIDER_H


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider ( juce::Graphics&,
                            int x, int y, int witdh, int height,
                            float sliderPosProportional,
                            float rotarySliderAngle,
                            float rotaryEndAngle,
                            juce::Slider&) override;
};

// ROTARY SLIDER CUSTOM
struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
            juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                         juce::Slider::TextEntryBoxPosition::NoTextBox),
            param(&rap),
            suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels() override
    {
        setLookAndFeel(nullptr);
    }

    struct LabelPos
    {
        float pos;
        juce::String label;
    };

    juce::Array<LabelPos> labels;

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;

private:

    LookAndFeel lnf;
    juce::RangedAudioParameter* param;
    juce::String suffix;



};


#endif //AUDIO_PLUGIN_EXAMPLE_ROTARYSLIDER_H
