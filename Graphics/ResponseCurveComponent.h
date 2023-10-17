//
// Created by Kai on 15.10.23.
//




#include "../Processor/PluginProcessor.h"
#include "FFT.h"
#include "PathGenerator.h"

//==============================================================================

struct PathProducer
{
    // FIFO

    juce::AudioBuffer<float> monoBuffer;


    PathProducer (SingleChannelSampleFifo<AudioPluginAudioProcessor::BlockType>& scsf) :
    leftChannelFifo(&scsf)
    {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);

        monoBuffer.setSize(1,leftChannelFFTDataGenerator.getFFTSize());

    }
    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() { return leftChannelFFTPath; }

private:
    // FIFO
    SingleChannelSampleFifo<AudioPluginAudioProcessor::BlockType>*  leftChannelFifo;

    // FFT

    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;

    AnalyzerPathGenerator<juce::Path> pathProducer;

    juce::Path leftChannelFFTPath;

};


//==============================================================================
// ResponseCurveComponent Object
struct ResponseCurveComponent: juce::Component,
                               juce::AudioProcessorParameter::Listener,
                               juce::Timer
{
    ResponseCurveComponent(AudioPluginAudioProcessor &, juce::MouseEvent mouseEvent);
    ~ResponseCurveComponent() ;

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

    juce::MouseEvent mouseEvent;

    juce::Image background;

    // ANALYZER
    PathProducer leftPathProducer, rightPathProducer;

};



