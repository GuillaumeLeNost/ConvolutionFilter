

#include "FastFourierTransformer.h"

//--------------------------------------------------------------

// simple fft class implementation

//--------------------------------------------------------------

// constructor

FastFourierTransformer::FastFourierTransformer (int fftSize) {
	
	data          = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fftSize);
	fft_result    = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fftSize);
	ifft_result   = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fftSize);
	
	plan_forward  = fftw_plan_dft_1d(fftSize, data, fft_result, FFTW_FORWARD, FFTW_MEASURE);
	plan_backward = fftw_plan_dft_1d(fftSize, data, ifft_result, FFTW_BACKWARD, FFTW_MEASURE);	
	
}

//-------------------------------------------------------------

// destructor

FastFourierTransformer::~FastFourierTransformer() {
	
	fftw_free(data);
	fftw_free(fft_result);
	fftw_free(ifft_result);
	fftw_destroy_plan(plan_forward);	
	fftw_destroy_plan(plan_backward);
	
}

//--------------------------------------------------------------

// fft conversion method

void FastFourierTransformer::processForward (float* channelData, fftw_complex* fftData, int dataSize, int fftSize) {
	
	for(i = 0; i < fftSize; i++) {
		
		if (i<dataSize){
			data[i][0] = channelData[i]; }      // stick your audio samples in here  
		else {
			data[i][0] = 0.0;					// zero padding for i>dataSize
		}
		data[i][1] = 0.0;                  // use this if your data is complex valued
	}
		
	fftw_execute(plan_forward);
	
	for (i = 0; i < fftSize; i++) {
		
		fftData[i][0] = fft_result[i][0] ;
		fftData[i][1] = fft_result[i][1] ;				
	}	
}

//--------------------------------------------------------------

// inverse fft conversion method

void FastFourierTransformer::processBackward (fftw_complex* fftData, float* ifftSamples, int fftSize) {
			
	for(i = 0; i < fftSize; i++) {
		
		data[i][0] = fftData[i][0];        // stick your fft data in here!
		data[i][1] = fftData[i][1];        // use this if your data is complex valued
	}	
	
	fftw_execute(plan_backward);
		
	for(i = 0; i < fftSize; i++) {
	
		ifftSamples[i] = ifft_result[i][0] / fftSize ;
		
		// fftw3 code is not normalized ! need to divide once in the fft/ifft process.
	}		
}


//--------------------------------------------------------------

//  multiply two arrays of fft_complex

fftw_complex* FastFourierTransformer::mult (fftw_complex* X, fftw_complex* Y, int fftSize) {
	for(i = 0; i < fftSize; i++) {
		data[i][0] = X[i][0] * Y[i][0] - X[i][1] * Y[i][1] ;
		data[i][1] = X[i][0] * Y[i][1] + X[i][1] * Y[i][0] ;
	}
	 return data ;
 }

/*
//--------------------------------------------------------------

//polar to cartesian conversion functions

double FastFourierTransformer::real (double phase, double mag) { 
	
	return cos(phase) * mag;
}



double FastFourierTransformer::imag (double phase, double mag) {
	
	return sin(phase) * mag ;	
}

//--------------------------------------------------------------

//cartesian to polar conversion functions

double FastFourierTransformer::magnitude (double x, double y) {
	
	return sqrt(y * y + x * x);	
}

double FastFourierTransformer::angle (double x, double y)  { 
	
    if (x > 0) { return atan(y/x); }
	if (x < 0 && y >= 0) {return atan(y/x) + M_PI; }
	if (x < 0 && y < 0) {return atan(y/x) - M_PI; }
	if (y > 0) { return M_PI / 2; }
	if (y < 0) { return -M_PI / 2; }
	
	return 0;	
}
*/