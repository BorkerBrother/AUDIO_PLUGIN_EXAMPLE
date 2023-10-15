//
// Created by Kai on 15.10.23.
//

#include "ResponseCurveComponent.h"

//==============================================================================
ResponseCurveComponent::ResponseCurveComponent(AudioPluginAudioProcessor& p) : processorRef (p),
leftChannelFifo(&processorRef.leftChannelFifo)
{
    const auto& params = processorRef.getParameters();
    for ( auto param : params )
    {
        param->addListener(this);
    }

    leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);

    monoBuffer.setSize(1,leftChannelFFTDataGenerator.getFFTSize());

    updateChain();

    startTimerHz(60);
}


ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = processorRef.getParameters();
    for ( auto param : params )
    {
        param->removeListener(this);
    }

}


void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue) {

    parametersChanged.set(true);
}


/*A Timer's timerCallback() method will be repeatedly called at a given interval. When you create a Timer object,
 it will do nothing until the startTimer() method is called, which will cause the message thread to start making
 callbacks at the specified interval, until stopTimer() is called or the object is deleted.
The time interval isn't guaranteed to be precise to any more than maybe 10-20ms, and the intervals may end up being much
 longer than requested if the system is busy. Because the callbacks are made by the main message thread, anything that
 blocks the message queue for a period of time will also prevent any timers from running until it can carry on.
 */

void ResponseCurveComponent::timerCallback() {


    juce::AudioBuffer<float> tempIncomingBuffer;

    while( leftChannelFifo->getNumCompleteBuffersAvailable() > 0 )
    {
        if ( leftChannelFifo->getAudioBuffer(tempIncomingBuffer) )
        {
            auto size = tempIncomingBuffer.getNumSamples();

            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0,0),
                                              monoBuffer.getReadPointer(0, size),
                                              monoBuffer.getNumSamples() - size);

            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0,monoBuffer.getNumSamples() - size),
                                                                         tempIncomingBuffer.getReadPointer(0,0),
                                                                         size);

            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer,-48.f);

            // If we can pull a buffer generate a path



        }
    }

    auto fftBounds = getAnalysisArea().toFloat();
    auto fftSize = leftChannelFFTDataGenerator.getFFTSize();

    const auto binWidth = processorRef.getSampleRate() / (double)fftSize;

    while ( leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0 )
    {
        std::vector<float> fftData;
        if (leftChannelFFTDataGenerator.getFFTData(fftData))
        {
            pathProducer.generatePath(fftData,
                                      fftBounds,
                                      fftSize,
                                      binWidth, -48.f);

        }

    }

    // While there are paths that can pull  -> pull as many as we can

    while(pathProducer.getNumPathsAvailable() )
    {
        pathProducer.getPath(leftChannelFFTPath);
    }

    if ( parametersChanged.compareAndSetBool(false,true) )
    {
        updateChain();
        // Repaint
        //repaint();
    }

    repaint();

}

void ResponseCurveComponent::updateChain()
{
    auto chainSettings = getChainSettings(processorRef.apvts);

    auto peakCoefficients = makePeakFilter(chainSettings, processorRef.getSampleRate());
    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients );

    auto lowCutCoefficients = makeLowCutFilter(chainSettings, processorRef.getSampleRate());
    auto highCutCoefficients = makeHighCutFilter(chainSettings, processorRef.getSampleRate());

    updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);

}

void ResponseCurveComponent::paint (juce::Graphics& g) {

    using namespace juce;


    g.drawImage(background,getLocalBounds().toFloat());
    // GET ResponseArea PIXEL
    auto responseArea =  getAnalysisArea();

    auto w = responseArea.getWidth();

    auto& lowCut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highCut = monoChain.get<ChainPositions::HighCut>();

    // Get sample Rate
    auto sampleRate = processorRef.getSampleRate();

    // VECTOR FOR DRAWING
    std::vector<double> mags;

    mags.resize(w);

    // Paint for every Pixel
    for (int i = 0; i < w; i++)
    {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

        //Peak
        if (! monoChain.isBypassed<ChainPositions::Peak>() )
            mag *= peak.coefficients->getMagnitudeForFrequency(freq,sampleRate);

        //Lowcut
        if (! lowCut.isBypassed<0>() )
            mag *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq,sampleRate);
        if (! lowCut.isBypassed<1>() )
            mag *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq,sampleRate);
        if (! lowCut.isBypassed<2>() )
            mag *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq,sampleRate);
        if (! lowCut.isBypassed<3>() )
            mag *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq,sampleRate);

        //HighCut
        if (! highCut.isBypassed<0>() )
            mag *= highCut.get<0>().coefficients->getMagnitudeForFrequency(freq,sampleRate);
        if (! highCut.isBypassed<1>() )
            mag *= highCut.get<1>().coefficients->getMagnitudeForFrequency(freq,sampleRate);
        if (! highCut.isBypassed<2>() )
            mag *= highCut.get<2>().coefficients->getMagnitudeForFrequency(freq,sampleRate);
        if (! highCut.isBypassed<3>() )
            mag *= highCut.get<3>().coefficients->getMagnitudeForFrequency(freq,sampleRate);

        mags[i] = Decibels::gainToDecibels(mag);
    }

    // BUILD PATH

    /*
    To use a path, you can create an empty one, then add lines and curves to it
    to create shapes, then it can be rendered by a Graphics context or used
    for geometric operations.
     */
    Path responseCurve;

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();

    auto map = [outputMin, outputMax](double input)
    {
        /* jmap - Remaps a value from a source range to a target range. */
        return jmap(input, -24.0, 24.0 , outputMin,outputMax);
    };

    // Start new Subpath with Magnetude (
    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));


    for ( size_t i = 1; i < mags.size(); i++)
    {
        responseCurve.lineTo(responseArea.getX()+ i, map(mags[i]));
    }

    // DRAW FFT

    leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(),
                                                                    responseArea.getY()));
    g.setColour(Colours::white);
    g.strokePath(leftChannelFFTPath,PathStrokeType(1.f));


    // DRAW BUTTON
    // TODO: Get Cutoff freq from peak and draw it

    g.setColour(Colours::orange);
    juce::Rectangle<float> pointArea ({});
    pointArea.setBounds(10,10,10,10);
    g.fillRect (pointArea);

    g.drawRoundedRectangle(getRenderArea().toFloat(),4.f,1.f);


    // DRAW RESPONSECURVE

    g.setColour(Colours::white);
    // Paint Path
    g.strokePath(responseCurve,PathStrokeType(2.f));

}


void ResponseCurveComponent::resized()
{
    using namespace juce;
    background = Image(Image::PixelFormat::RGB, getWidth(),getHeight(), true);

    Graphics g(background);

    Array<float> freqs
            {
                    20, 30, /*40,*/ 50,100,
                    200,  300,/*400,*/ 500,1000,
                    2000, 3000, /*,4000 */ 5000,10000,
                    20000
            };

    auto renderArea = getAnalysisArea();

    auto left = getRenderArea().getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    Array<float> xs;
    for ( auto f : freqs )
    {
        auto normX = mapFromLog10(f,20.f,20000.f);
        xs.add(left + width * normX);
    }

    g.setColour(Colours::dimgrey);

    // DRAW Vertical Lines
    for ( auto x : xs)
    {
        g.drawVerticalLine(x,top, bottom);

    }

    Array<float> gain
            {
                    -24, -12, 0, 12, 24
            };


    // DRAW Horizontal Lines
    for ( auto gDb : gain)
    {
        auto y = jmap(gDb,
                      -24.f,
                      24.f,
                      float(bottom),
                      float(top));

        g.setColour(gDb == 0.f ? (Colours::orange) : Colours::darkgrey);

        g.drawHorizontalLine(y,left,right);

    }

    g.setColour(Colours::white);

    const int fontHeight = 10;
    g.setFont(fontHeight);

    for ( int i = 0; i < freqs.size(); ++i)
    {
        auto f = freqs[i];
        auto x = xs[i];

        bool addK = false;
        String str;
        if( f > 999.f )
        {
            addK = true;
            f /= 1000.f;
        }

        str << f;

        if( addK)

            str << "k";
        str << "Hz";

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;
        r.setSize(textWidth,fontHeight);
        r.setCentre(x,0);
        r.setY(1);

        g.drawFittedText(str,r,juce::Justification::centred,1);


    }

    // DRAW DB LINES String

    for ( auto gDb : gain) {
        auto y = jmap(gDb,
                      -24.f,
                      24.f,
                      float(bottom),
                      float(top));

        String str;
        if(gDb > 0)
            str << "+";

        str << gDb;

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), y);

        g.setColour(gDb == 0.f ? (Colours::orange) : Colours::lightgrey);

        g.drawFittedText(str,r,juce::Justification::centred,1);

        str.clear();

        str << (gDb -24.f);

        r.setX(1);
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, fontHeight);
        g.setColour(Colours::lightgrey);

        g.drawFittedText(str,r,juce::Justification::centred,1);


    }



}

juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
    auto bounds = getLocalBounds();

    bounds.reduce(20, 8);

    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);

    return bounds;
}

juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);

    return bounds;
}
