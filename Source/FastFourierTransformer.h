

#define USE_MATH_DEFINES

#include "FFTW/fftw3.h"
#include "math.h"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#ifndef __FASTFOURIERTRANSFORMER_H_9F2E8A4F__
#define __FASTFOURIERTRANSFORMER_H_9F2E8A4F__

// fft class

class FastFourierTransformer {

// class methods
	
public:
	
	FastFourierTransformer (int fftSize);	//constructor
	~FastFourierTransformer();				//destructor
	
	
	//fft conversion
	
	void processForward	   (float* channelData, fftw_complex* fftData, int dataSize, int fftSize);
	void processBackward   (fftw_complex* fftData, float* channelData, int fftSize);
	
	// fft mult
	fftw_complex* mult  (fftw_complex* X, fftw_complex* Y, int fftSize);
						
// member variables
	
private:
	
	fftw_complex    *data, *fft_result, *ifft_result,;	
	fftw_plan       plan_forward, plan_backward;
	int             i;
	
};


#endif  // __FASTFOURIERTRANSFORMER_H_9F2E8A4F__
