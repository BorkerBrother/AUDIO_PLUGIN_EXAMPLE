//
// Created by Kai on 15.10.23.
//

#include "PeakFilter.h"

Coefficents makePeakFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(            // IIR peak Update
            sampleRate,
            chainSettings.peakFreq,
            chainSettings.peakQuality,
            juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));

}

