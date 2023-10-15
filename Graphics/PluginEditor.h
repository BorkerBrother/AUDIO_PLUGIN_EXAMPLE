#pragma once

#include "../Processor/PluginProcessor.h"
#include "../Graphics/RotarySlider.h"
#include "../Graphics/ResponseCurveComponent.h"

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
    RotarySliderWithLabels
    peakFreqSlider,
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
