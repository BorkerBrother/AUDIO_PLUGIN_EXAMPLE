//
// Created by Kai on 15.10.23.
//
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Setting.h"


#ifndef AUDIO_PLUGIN_EXAMPLE_CUTFILTER_H
#define AUDIO_PLUGIN_EXAMPLE_CUTFILTER_H


ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);


using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;                //

// MONOCHAIN
using MonoChain = juce::dsp::ProcessorChain<CutFilter , Filter, CutFilter>;

inline auto makeLowCutFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
            chainSettings.lowCutFreq,
            sampleRate,
            2*(chainSettings.lowCutSlope +1));
}

inline auto makeHighCutFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
            chainSettings.highCutFreq,
            sampleRate,
            2*(chainSettings.highCutSlope +1));
}


#endif //AUDIO_PLUGIN_EXAMPLE_CUTFILTER_H
