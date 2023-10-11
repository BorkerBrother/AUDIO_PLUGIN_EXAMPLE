#pragma once

#include "PluginProcessor.h"


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider ( juce::Graphics&,
                            int x, int y, int witdh, int height,
                            float sliderPosProportional,
                            float rotarySliderAngle,
                            float rotaryEndAngle,
                            juce::Slider& ) override { }
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

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override { };
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;

private:
    LookAndFeel lnf;
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

//==============================================================================

// ResponseCurveComponent Object
struct ResponseCurveComponent: juce::Component,
        juce::AudioProcessorParameter::Listener,
        juce::Timer
{
    ResponseCurveComponent(AudioPluginAudioProcessor&);
    ~ResponseCurveComponent();

    void parameterValueChanged (int parameterIndex, float newValue) override ;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override { };
    void timerCallback() override;
    void paint(juce::Graphics& g) override;

private:

    AudioPluginAudioProcessor& processorRef;
    /*
    Objects of atomic types are the only C++ objects that are free from data races;
     that is, if one thread writes to an atomic object while another thread reads from it, the behavior is well-defined.
    In addition, accesses to atomic objects may establish inter-thread synchronization
     and order non-atomic memory accesses as specified by std::memory_order.
*/
    juce::Atomic<bool> parametersChanged {false};

    MonoChain monoChain;

};

//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor

{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
    //
    AudioPluginAudioProcessor& processorRef;

    // Create ROTARY SLIDER
    RotarySliderWithLabels peakFreqSlider,
    peakGainSlider,
    peakQualitySlider,
    lowCutFreqSlider,
    highCutFreqSlider,
    lowCutSlopeSlider,
    highCutSlopeSlider;

    ResponseCurveComponent responseCurveComponent;

    // SLIDER ATTACHMENT
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment peakFreqSliderAttachment,
            peakGainSliderAttachment,
            peakQualitySliderAttachment,
            lowCutFreqSliderAttachment,
            highCutFreqSliderAttachment,
            lowCutSlopeSliderAttachment,
            highCutSlopeSliderAttachment;


    // VECTOR FOR SLIDERS
    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
