/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
ConvolutionAudioProcessor::ConvolutionAudioProcessor()
{
	fft = NULL;
}

ConvolutionAudioProcessor::~ConvolutionAudioProcessor()
{
	
	deleteAndZero(fft) ;
}

//==============================================================================
const String ConvolutionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int ConvolutionAudioProcessor::getNumParameters()
{
    return 1; // Wet  ratio
}

float ConvolutionAudioProcessor::getParameter (int index)
{
	if (index==0)
		return wet ; // 100% wet when param=1
    return 0.0f;
}

void ConvolutionAudioProcessor::setParameter (int index, float newValue)
{
	//set wet to value received from MainPanel at index 0
	if (index == 0)
		wet = newValue;
}

const String ConvolutionAudioProcessor::getParameterName (int index)
{
	if (index==0) 
		return "wet";
    return String::empty;
}

const String ConvolutionAudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String ConvolutionAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String ConvolutionAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool ConvolutionAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool ConvolutionAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool ConvolutionAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ConvolutionAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int ConvolutionAudioProcessor::getNumPrograms()
{
    return 0;
}

int ConvolutionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ConvolutionAudioProcessor::setCurrentProgram (int index)
{
}

const String ConvolutionAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void ConvolutionAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ConvolutionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	// The samplesPerBlock is an estimation of the average nb of samples per buffer
	nbFilters = 2 ; // number of filters (1 for mono, 2 for stereo)
	filterLength = 2048;	 // 2048 over 44100 is about 50ms length
	filterSamples = new float[filterLength];
	
	// NFFT for convolution
	if (samplesPerBlock>filterLength) {
	 nfft=samplesPerBlock *2 ; 
	}
	else {
		nfft = filterLength *2 ;
	}
	
	// 2D ola Buffer
	convSize = filterLength + samplesPerBlock - 1 ;
	olaBuffer = new float*[nbFilters] ;
	for (int k=0; k<nbFilters; k++) {
		olaBuffer[k] = new float[nfft];
		for (int i=0; i<nfft; i++) {
			olaBuffer[k][i] = 0;
		}
	}
	
	// circular buffer for the dry signal to account for latency
	dryBufferSize = filterLength/2 + samplesPerBlock ;
	dryBuffer = new float[dryBufferSize] ; // only works if samplesPerBlock remains stable
	for (int i=0 ; i<dryBufferSize ; i++) {
		dryBuffer[i] = 0 ;
	}
	
	// Plan initialisation
	if (fft == NULL){
		fft = new FastFourierTransformer(nfft) ;
	}	
	ifftSamples = new float[nfft];
	fftData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nfft) ; // will be used to store the unfiltered FFT of the incoming signal
	fftDataFilt = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nfft) ; // used to store the filtered FFT
	 
	 // Create Filter 0 and get its Fourier transform
	for (int i=0; i<filterLength; i++) {
		filterSamples[i] = 0;
	}
	filterSamples[1] = 1 ;
	filterSamples[100] = -0.8 ;
	filterSamples[1205] = 0.5 ;
	
	fftH0 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nfft) ;
	fft->processForward(filterSamples, fftH0, filterLength, nfft) ;
	
	// Create Filter 1 and get its Fourier transform
	for (int i=0; i<filterLength; i++) {
		filterSamples[i] = 0;
	}
	filterSamples[1] = 1 ;
	filterSamples[1670] = -0.8 ;
	filterSamples[340] = 0.5 ;
	fftH1 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nfft) ;
	fft->processForward(filterSamples, fftH1, filterLength, nfft) ;
	
	// Params init
	oldwet = wet ;
}


void ConvolutionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
	fftw_free(fftData) ;
	fftw_free(fftDataFilt) ;
	fftw_free(fftH0) ;
	fftw_free(fftH1) ;
	delete [] ifftSamples ;	
	delete [] filterSamples ;
	delete [] dryBuffer ;
	
	for (int k = 0; k < nbFilters; k++)
		delete [] olaBuffer[k];
	delete [] olaBuffer;
	
}

void ConvolutionAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Warning : buffer.getNumSamples can vary at each callback !
	
    for (int channel = 0; channel < getNumInputChannels(); ++channel)
    {
        float* channelData = buffer.getSampleData (channel) ;
		int N = getNumOutputChannels() ;
		int bufsize = buffer.getNumSamples() ;
		
		
		// Fill up the dry circular Buffer, used to account for latency of the wet signals when summing dry and wet signals at the end
		for (int i = 0 ; i<dryBufferSize-bufsize ; i++)
		{
			dryBuffer[i] = dryBuffer[i+bufsize] ;
		}
		for (int i = (dryBufferSize-bufsize) ; i<dryBufferSize ; i++)
		{
			dryBuffer[i] = channelData[i-dryBufferSize+bufsize] ;
		}
		
		// Do the FFT
		fft->processForward(channelData, fftData, bufsize, nfft) ;
		
		// zeros the elements that have been previously outputted, put them at the end of the ola buffer
		for (int k=0; k<nbFilters; k++) {
			for (int i = 0 ; i<nfft-bufsize ; i++)
			{
				olaBuffer[k][i] = olaBuffer[k][i+bufsize] ;
			}
			for (int i = (nfft-bufsize) ; i<nfft ; i++)
			{
				olaBuffer[k][i] = 0.0 ;
			} 
		}
		
		// Multiply FFT with filter0
		for(int i = 0; i < nfft; i++) {
			fftDataFilt[i][0]  = fftData[i][0] * fftH0[i][0] - fftData[i][1] * fftH0[i][1] ;
			fftDataFilt[i][1] = fftData[i][0] * fftH0[i][1] + fftData[i][1] * fftH0[i][0] ;
		}

		// Reverse FFT
		fft->processBackward(fftDataFilt, ifftSamples, nfft) ;
		
		// Add to overlap-add (ola) buffer0
		//	for (int i = 0 ; i<bufsize ; i++) // moins bien car la taille du buffer peut changer...
		for (int i = 0 ; i<nfft ; i++) {
			olaBuffer[0][i] += ifftSamples[i] ;
		}
		
		// Multiply FFT with filter1
		for(int i = 0; i < nfft; i++) {
			fftDataFilt[i][0]  = fftData[i][0] * fftH1[i][0] - fftData[i][1] * fftH1[i][1] ;
			fftDataFilt[i][1] = fftData[i][0] * fftH1[i][1] + fftData[i][1] * fftH1[i][0] ;
		}
		
		// Reverse FFT
		fft->processBackward(fftDataFilt, ifftSamples, nfft) ;
		
		// Add to overlap-add (ola) buffer1
		for (int i = 0 ; i<nfft ; i++) {
			olaBuffer[1][i] += ifftSamples[i] ;
		}
		
		// output buffer clean
		for (int i = 0; i < N ; ++i) {
			buffer.clear (i, 0, bufsize) ; 
		}
		
		// Left signal
		buffer.addFromWithRamp(0, 0, olaBuffer[0], bufsize, oldwet, wet) ; // wet signal
		buffer.addFromWithRamp(0, 0, dryBuffer, bufsize, (1.0-oldwet), (1.0-wet)) ; // dry signal 
		
		// Right signal
		buffer.addFromWithRamp(1, 0, olaBuffer[1], bufsize, oldwet, wet) ; // wet signal
		buffer.addFromWithRamp(1, 0, dryBuffer, bufsize, (1.0-oldwet), (1.0-wet)) ; // dry signal
		
		// update parameters
		oldwet = wet ;
    }

}

//==============================================================================
bool ConvolutionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ConvolutionAudioProcessor::createEditor()
{
    return new ConvolutionAudioProcessorEditor (this);
}

//==============================================================================
void ConvolutionAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ConvolutionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ConvolutionAudioProcessor();
}
