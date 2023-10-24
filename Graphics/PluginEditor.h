#pragma once

#include "RotarySlider.h"
#include "ResponseCurveComponent.h"


//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor

{
public:
    AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
    //
    AudioPluginAudioProcessor& processorRef;

    juce::MouseEvent mouseEvent;

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

    juce::ToggleButton  lowcutBypasseButton,
                        highcutBypassButton,
                        peakBypassedButton,
                        analyzerEnableButton;

    using ButtonAttachment = APVTS::ButtonAttachment;

    ButtonAttachment    lowcutBypasseButtonAttachment,
                        highcutBypassButtonAttachment,
                        peakBypassedButtonAttachment,
                        analyzerEnableButtonAttachment;

    // VECTOR FOR SLIDERS
    std::vector<juce::Component*> getComps();



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
