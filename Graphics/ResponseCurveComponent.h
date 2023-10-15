//
// Created by Kai on 15.10.23.
//




#include "../Processor/PluginProcessor.h"
#include "FFT.h"
#include "PathGenerator.h"


//==============================================================================

// ResponseCurveComponent Object
struct ResponseCurveComponent: juce::Component,
                               juce::AudioProcessorParameter::Listener,
                               juce::Timer
{
    ResponseCurveComponent(AudioPluginAudioProcessor&);
    ~ResponseCurveComponent() ;

    void parameterValueChanged (int parameterIndex, float newValue) override ;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override { }
    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

    juce::Rectangle<int> getRenderArea();

    juce::Rectangle<int> getAnalysisArea();

    // FIFO

    juce::AudioBuffer<float> monoBuffer;

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

    // FIFO
    SingleChannelSampleFifo<AudioPluginAudioProcessor::BlockType>*  leftChannelFifo;

    // FFT

    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;

    AnalyzerPathGenerator<juce::Path> pathProducer;

    juce::Path leftChannelFFTPath;

};



