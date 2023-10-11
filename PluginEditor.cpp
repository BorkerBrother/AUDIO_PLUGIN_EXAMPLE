#include "PluginProcessor.h"
#include "PluginEditor.h"

ResponseCurveComponent::ResponseCurveComponent(AudioPluginAudioProcessor& p) : processorRef (p)
{
    const auto& params = processorRef.getParameters();
    for ( auto param : params )
    {
        param->addListener(this);
    }

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

    if ( parametersChanged.compareAndSetBool(false,true) )
    {

        auto chainSettings = getChainSettings(processorRef.apvts);

        auto peakCoefficients = makePeakFilter(chainSettings, processorRef.getSampleRate());
        updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients );

        auto lowCutCoefficients = makeLowCutFilter(chainSettings, processorRef.getSampleRate());
        auto highCutCoefficients = makeHighCutFilter(chainSettings, processorRef.getSampleRate());

        updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
        updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);

        // Repaint
        repaint();
    }
}


void ResponseCurveComponent::paint (juce::Graphics& g) {

    using namespace juce;

    // GET ResponseArea PIXEL
    auto responseArea = getLocalBounds();
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

    // TODO: Get Cutoff freq from peak and draw it

    g.setColour(Colours::orange);
    juce::Rectangle<float> pointArea ({});
    pointArea.setBounds(10,10,10,10);
    g.fillRect (pointArea);



    //g.setColour(Colours::orange);
    g.drawRoundedRectangle(responseArea.toFloat(),4.f,1.f);


    g.setColour(Colours::white);
    // Paint Path
    g.strokePath(responseCurve,PathStrokeType(2.f));

}


//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),

      peakFreqSlider(*processorRef.apvts.getParameter("Peak Freq"), "Hz"),
      peakGainSlider(*processorRef.apvts.getParameter( "Peak Gain"), "db"),
      peakQualitySlider(*processorRef.apvts.getParameter("Peak Quality"), "Q"),
      lowCutFreqSlider(*processorRef.apvts.getParameter( "LowCut Freq"), "Hz"),
      lowCutSlopeSlider(*processorRef.apvts.getParameter( "HighCut Freq"), "Hz"),
      highCutFreqSlider(*processorRef.apvts.getParameter( "LowCut Slope"), "dB/Oct"),
      highCutSlopeSlider(*processorRef.apvts.getParameter( "HighCut Slope"), "dB/Oct"),

      responseCurveComponent(processorRef),

      peakFreqSliderAttachment(processorRef.apvts, "Peak Freq", peakFreqSlider),
      peakGainSliderAttachment(processorRef.apvts, "Peak Gain", peakGainSlider),
      peakQualitySliderAttachment(processorRef.apvts, "Peak Quality", peakQualitySlider),
      lowCutFreqSliderAttachment(processorRef.apvts, "LowCut Freq", lowCutFreqSlider),
      lowCutSlopeSliderAttachment(processorRef.apvts, "HighCut Freq", lowCutSlopeSlider),
      highCutFreqSliderAttachment(processorRef.apvts, "LowCut Slope", highCutFreqSlider),
      highCutSlopeSliderAttachment(processorRef.apvts, "HighCut Slope", highCutSlopeSlider)

{

    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }

    setSize (800, 500);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{

}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g) {
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)

    g.fillAll(Colours::black);

}


void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    responseCurveComponent.setBounds(responseArea);

    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() *0.5);

    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop((lowCutArea.getHeight() * 0.5)));
    lowCutSlopeSlider.setBounds(lowCutArea);

    highCutFreqSlider.setBounds(highCutArea.removeFromTop((highCutArea.getHeight() * 0.5)));
    highCutSlopeSlider.setBounds(highCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight()* 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight()* 0.5));
    peakQualitySlider.setBounds(bounds);
}





std::vector<juce::Component*> AudioPluginAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider,
        &responseCurveComponent
    };
}