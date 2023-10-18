#include <juce_gui_extra/misc/juce_LiveConstantEditor.h>
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor (&p), processorRef (p),

      peakFreqSlider(*processorRef.apvts.getParameter("Peak Freq"), "Hz"),
      peakGainSlider(*processorRef.apvts.getParameter( "Peak Gain"), "db"),
      peakQualitySlider(*processorRef.apvts.getParameter("Peak Quality"), "Q"),
      lowCutFreqSlider(*processorRef.apvts.getParameter( "LowCut Freq"), "Hz"),
      highCutFreqSlider(*processorRef.apvts.getParameter( "HighCut Freq"), "Hz"),
      lowCutSlopeSlider(*processorRef.apvts.getParameter( "LowCut Slope"), "dB/Oct"),
      highCutSlopeSlider(*processorRef.apvts.getParameter( "HighCut Slope"), "dB/Oct"),

      responseCurveComponent(processorRef,mouseEvent),

      peakFreqSliderAttachment(processorRef.apvts, "Peak Freq", peakFreqSlider),
      peakGainSliderAttachment(processorRef.apvts, "Peak Gain", peakGainSlider),
      peakQualitySliderAttachment(processorRef.apvts, "Peak Quality", peakQualitySlider),
      lowCutFreqSliderAttachment(processorRef.apvts, "LowCut Freq", lowCutFreqSlider),
      highCutFreqSliderAttachment(processorRef.apvts, "HighCut Freq", highCutFreqSlider),
      lowCutSlopeSliderAttachment(processorRef.apvts, "LowCut Slope", lowCutSlopeSlider),
      highCutSlopeSliderAttachment(processorRef.apvts, "HighCut Slope", highCutSlopeSlider), mouseEvent(mouseEvent) {



    // LABELS
    peakFreqSlider.labels.add({0.f, "20Hz"});
    peakFreqSlider.labels.add({1.f, "20kHz"});
    lowCutFreqSlider.labels.add({0.f, "20Hz"});
    lowCutFreqSlider.labels.add({1.f, "20kHz"});
    highCutFreqSlider.labels.add({0.f, "20Hz"});
    highCutFreqSlider.labels.add({1.f, "20kHz"});
    peakGainSlider.labels.add({0.f, "-24dB"});
    peakGainSlider.labels.add({1.f, "+24dB"});
    peakQualitySlider.labels.add({0.f, "0.1Q"});
    peakQualitySlider.labels.add({1.f, "10Q"});


    //for (auto* comp : getComps())
    //{
    //    addAndMakeVisible(comp);
    //    // TODO: Make Slope a ComboBox
    //}

    addAndMakeVisible(responseCurveComponent);
    addAndMakeVisible(peakGainSlider);
    addAndMakeVisible(peakFreqSlider);
    addAndMakeVisible(peakQualitySlider);


    setSize (800, 500);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{

}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g) {
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)

    g.fillAll(Colours::black);

}


void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..


    auto bounds = getLocalBounds();
    float hRatio = JUCE_LIVE_CONSTANT(100) / 100.f;
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * hRatio);

    responseCurveComponent.setBounds(responseArea);

    //auto newBounds = setBounds(400,500,100,30);

    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() *0.5);

    //lowCutFreqSlider.setBounds(lowCutArea.removeFromTop((lowCutArea.getHeight() * 0.5)));
    //lowCutSlopeSlider.setBounds(lowCutArea);

    //highCutFreqSlider.setBounds(highCutArea.removeFromTop((highCutArea.getHeight() * 0.5)));
    //highCutSlopeSlider.setBounds(highCutArea);

    //peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight()* 0.33));

    peakGainSlider.setBounds(300,390,200,100);
    peakFreqSlider.setBounds(240,400,100,90);
    peakQualitySlider.setBounds(460,400,100,90);



    //peakQualitySlider.setBounds(bounds);
}





std::vector<juce::Component*> AudioPluginAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider,
        &responseCurveComponent
    };
}