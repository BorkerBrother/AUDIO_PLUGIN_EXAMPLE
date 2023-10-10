#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

enum Slope {
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};

enum ChainPositions
{
    LowCut,
    Peak,
    HighCut
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


private:





    // USING FILTER
    using Filter = juce::dsp::IIR::Filter<float>;                                               // Set Up Filter
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;                //

    // MONOCHAIN
    using MonoChain = juce::dsp::ProcessorChain<CutFilter , Filter, CutFilter>;

    // COEFFICIENTS
    using Coefficents = Filter::CoefficientsPtr;

    // DESIGN MONO CHAINS
    MonoChain leftChain, rightChain;


    // TEMPLATE UPDATE COEFFICIENTS
    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType& chain, const CoefficientType& coefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }

    // TEMPLATE UPDATE CUTFILTER
    template<typename ChainType, typename CoefficientType>
    void updateCutFilter (ChainType& chainType,
                          const CoefficientType& coefficients,
                          const Slope& slope)
    {


        chainType.template setBypassed<0>(true);
        chainType.template setBypassed<1>(true);
        chainType.template setBypassed<2>(true);
        chainType.template setBypassed<3>(true);

        switch(slope){

            case Slope_48:
            {
                update<3>(chainType, coefficients);
            }
            case Slope_36:
            {
                update<2>(chainType, coefficients);
            }
            case Slope_24:
            {
                update<1>(chainType, coefficients);
            }
            case Slope_12:
            {
                update<0>(chainType, coefficients);
            }
        }
    }

    void updateLowCutFilters(const ChainSettings& chainSettings);
    void updateHighCutFilters(const ChainSettings& chainSettings);
    void updatePeakFilter(const ChainSettings& chainSettings);

    static void updateCoefficients (Coefficents& old, const Coefficents& replacements);

    void updateFilters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
