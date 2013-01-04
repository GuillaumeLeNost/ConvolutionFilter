/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __PLUGINPROCESSOR_H_75683C3E__
#define __PLUGINPROCESSOR_H_75683C3E__

#include "../JuceLibraryCode/JuceHeader.h"
#include "FastFourierTransformer.h"


//==============================================================================
/**
*/
class ConvolutionAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    ConvolutionAudioProcessor();
    ~ConvolutionAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConvolutionAudioProcessor);
	
	int filterLength, nfft, convSize, nbFilters, dryBufferSize ;
	float *filterSamples, *ifftSamples ;
	float **olaBuffer ; // 2D overlap Add buffer
	float *dryBuffer ; // 1D circular buffer for LF
	float oldwet, wet;
	FastFourierTransformer *fft ;
	fftw_complex *fftData, *fftDataFilt, *fftH0, *fftH1 ;
};

#endif  // __PLUGINPROCESSOR_H_75683C3E__
