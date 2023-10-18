//
// Created by Kai on 15.10.23.
//

#ifndef AUDIO_PLUGIN_EXAMPLE_PATHGENERATOR_H
#define AUDIO_PLUGIN_EXAMPLE_PATHGENERATOR_H

#include <juce_audio_processors/juce_audio_processors.h>

template<typename PathType>
struct AnalyzerPathGenerator
{
    /*
     converts 'renderData[]' into a juce::Path
     */
    void generatePath(const std::vector<float>& renderData,
                      juce::Rectangle<float> fftBounds,
                      int fftSize,
                      float binWidth,
                      float negativeInfinity)
    {
        auto top = fftBounds.getY();
        auto bottom = fftBounds.getHeight();
        auto width = fftBounds.getWidth();

        int numBins = (int)fftSize / 2;

        PathType p;
        p.preallocateSpace(3 * (int)fftBounds.getWidth());
        auto map = [bottom, top, negativeInfinity](float v) {
            return juce::jmap(v, negativeInfinity, 0.f, float(bottom + 10), top);
        };

        auto y = map(renderData[0]);
        jassert(!std::isnan(y) && !std::isinf(y));
        if (!std::isnan(y) && !std::isinf(y)) {
            p.startNewSubPath(0, y);
        }

        const int pathResolution = 1;

        for (int binNum = 1; binNum < numBins; binNum += pathResolution)
        {
            float amplitude = renderData[binNum];
            float x = juce::mapFromLog10(binNum * binWidth, 20.0f, 20000.0f); // X-Koordinate basierend auf der Frequenz
            float y = map(amplitude); // Y-Koordinate basierend auf der Amplitude

            if (!std::isnan(y) && !std::isinf(y))
            {
                int binX = std::floor(x * width);
                p.cubicTo(binX  , y -15, binX  , y, binX  , y );
                //p.fillRect(juce::Colours::red);
            }
        }

        pathFifo.push(p);
    }

    int getNumPathsAvailable() const
    {
        return pathFifo.getNumAvailableForReading();
    }

    bool getPath(PathType& path)
    {
        return pathFifo.pull(path);
    }
private:
    Fifo<PathType> pathFifo;
};

#endif //AUDIO_PLUGIN_EXAMPLE_PATHGENERATOR_H
