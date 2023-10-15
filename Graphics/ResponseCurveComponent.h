//
// Created by Kai on 15.10.23.
//

#include "../Processor/PluginProcessor.h"

#ifndef AUDIO_PLUGIN_EXAMPLE_RESPONSECURVECOMPONENT_H
#define AUDIO_PLUGIN_EXAMPLE_RESPONSECURVECOMPONENT_H


//==============================================================================

// ResponseCurveComponent Object
struct ResponseCurveComponent: juce::Component,
                               juce::AudioProcessorParameter::Listener,
                               juce::Timer
{
    ResponseCurveComponent(AudioPluginAudioProcessor&);
    ~ResponseCurveComponent() override;

    void parameterValueChanged (int parameterIndex, float newValue) override ;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override { }
    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

    juce::Rectangle<int> getRenderArea();

    juce::Rectangle<int> getAnalysisArea();

private:

    AudioPluginAudioProcessor& processorRef;
    /*
    Objects of atomic types are the only C++ objects that are free from data races;
     that is, if one thread writes to an atomic object while another thread reads from it, the behavior is well-defined.
    In addition, accesses to atomic objects may establish inter-thread synchronization
     and order non-atomic memory accesses as specified by std::memory_order.
*/
    juce::Atomic<bool> parametersChanged {false};

    void updateChain();

    MonoChain monoChain;

    juce::Image background;

};


#endif //AUDIO_PLUGIN_EXAMPLE_RESPONSECURVECOMPONENT_H
