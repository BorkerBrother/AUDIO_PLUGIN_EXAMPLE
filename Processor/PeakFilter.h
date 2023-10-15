//
// Created by Kai on 15.10.23.
//
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Setting.h"

#ifndef AUDIO_PLUGIN_EXAMPLE_PEAKFILTER_H
#define AUDIO_PLUGIN_EXAMPLE_PEAKFILTER_H


using Filter = juce::dsp::IIR::Filter<float>;

// COEFFICIENTS
using Coefficents = Filter::CoefficientsPtr;

void updateCoefficients (Coefficents& old, const Coefficents& replacements);


Coefficents  makePeakFilter(const ChainSettings& chainSettings, double sampleRate);



#endif //AUDIO_PLUGIN_EXAMPLE_PEAKFILTER_H
