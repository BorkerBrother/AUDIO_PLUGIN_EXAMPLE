#pragma once

#include "PluginProcessor.h"

// ROTARY SLIDER
struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
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
    CustomRotarySlider peakFreqSlider,
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
