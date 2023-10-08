#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

enum Slope {
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};

struct ChainSettings
{
    float peakFreq{0}, peakGainInDecibels{0}, peakQuality{1.f};
    float lowCutFreq {0}, highCutFreq{0};

    Slope lowCutSlope {Slope::Slope_12}, highCutSlope {Slope::Slope_12};

};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);


//==============================================================================
class AudioPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts {*this, nullptr,"Parameters",createParameterLayout()};

    juce::AudioProcessorValueTreeState test {*this, nullptr,};



private:

    using Filter = juce::dsp::IIR::Filter<float>;                                               // Set Up Filter

    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;                //

    using MonoChain = juce::dsp::ProcessorChain<CutFilter , Filter, CutFilter>;

    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
        LowCut,
        Peak,
        HighCut
    };

    void updatePeakFilter(const ChainSettings& chainSettings);

    using Coefficents = Filter::CoefficientsPtr;
    static void updateCoefficients (Coefficents& old, const Coefficents& replacements);



    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType& chain, const CoefficientType& coefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter (ChainType& leftLowCut,
                          const CoefficientType& coefficients,
                          const Slope& slope)
    {


        leftLowCut.template setBypassed<0>(true);
        leftLowCut.template setBypassed<1>(true);
        leftLowCut.template setBypassed<2>(true);
        leftLowCut.template setBypassed<3>(true);

        switch(slope){

            case Slope_48:
            {
                update<3>(leftLowCut, coefficients);
            }
            case Slope_36:
            {
                update<2>(leftLowCut, coefficients);
            }
            case Slope_24:
            {
                update<1>(leftLowCut, coefficients);
            }
            case Slope_12:
            {
                update<0>(leftLowCut, coefficients);
            }

            /*
            case Slope_12: {

                *leftLowCut.template get<0>().coefficients = *cutCoefficion[0];
                leftLowCut.template setBypassed<0>(false);
                break;
            }
            case Slope_24:
            {
                *leftLowCut.template get<0>().coefficients = * cutCoefficion[0];
                leftLowCut.template setBypassed<0>(false);
                *leftLowCut.template get<1>().coefficients = * cutCoefficion[1];
                leftLowCut.template setBypassed<1>(false);
                break;
            }
            case Slope_36:
            {
                *leftLowCut.template get<0>().coefficients = * cutCoefficion[0];
                leftLowCut.template setBypassed<0>(false);
                *leftLowCut.template get<1>().coefficients = * cutCoefficion[1];
                leftLowCut.template setBypassed<1>(false);
                *leftLowCut.template get<2>().coefficients = * cutCoefficion[2];
                leftLowCut.template setBypassed<2>(false);
                break;
            }
            case Slope_48:
            {
                *leftLowCut.template get<0>().coefficients = * cutCoefficion[0];
                leftLowCut.template setBypassed<0>(false);
                *leftLowCut.template get<1>().coefficients = * cutCoefficion[1];
                leftLowCut.template setBypassed<1>(false);
                *leftLowCut.template get<2>().coefficients = * cutCoefficion[2];
                leftLowCut.template setBypassed<2>(false);
                *leftLowCut.template get<3>().coefficients = * cutCoefficion[3];
                leftLowCut.template setBypassed<3>(false);
                break;
            }
        */
        }

    }

    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);

    void updateFilters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
