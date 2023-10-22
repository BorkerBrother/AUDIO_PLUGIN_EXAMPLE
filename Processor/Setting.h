//
// Created by Kai on 15.10.23.
//

#ifndef AUDIO_PLUGIN_EXAMPLE_SETTING_H
#define AUDIO_PLUGIN_EXAMPLE_SETTING_H

enum Channel
{
    Right,  // Effectively 0
    Left    // Effectively 1
};

enum ChainPositions
{
    LowCut,
    Peak,
    HighCut
};


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

    bool lowCutBypassed { false}, peakBypassed { false }, highCutBypassed { false};


};

#endif //AUDIO_PLUGIN_EXAMPLE_SETTING_H
