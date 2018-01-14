#include <fftw3.h>
#include <math.h>
#include "MfccExtractor.h"
#include "SoundFile.h"
#include "inverse_mfcc.h"

using std::vector;

void MfccExtractor::Extract(vector<double>& feature_vec, vector<double>& input, SoundFile& sound) {
	int size = input.size();

	fftw_complex* in(static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * size)));
	fftw_complex* out(static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * size)));
	fftw_plan p(fftw_plan_dft_1d(input.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE));
	for (size_t j = 0; j < size; ++j) {
		in[j][0] = input[j];
		in[j][1] = 0;
	}
	fftw_execute(p);
	size /= 2;
	double* power_spectrum = new double[size];
	for (int i = 0; i < size; ++i) {
		power_spectrum[i] = sqrt(powf(out[i][0], 2) + powf(out[i][1], 2));
	}
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);

	double rate = sound.getSamplingRate();
	double max_freq = rate / 2;
	int nfft = windowSize * rate;

	mfcc.Init(count, nfft, 0.0, max_freq, rate);
	inverseMfcc.Init(count, nfft, 0.0, max_freq, rate);

	mfcc.SetSpectrumData(power_spectrum);
	mfcc.GetLogCoefficents(feature_vec);

	inverseMfcc.SetSpectrumData(power_spectrum);
	inverseMfcc.GetLogCoefficents(feature_vec);
	delete[] power_spectrum;
}

void MfccExtractor::Extract(vector<double>& feature_vec, vector<double>& input) {
	int size = input.size();

	fftw_complex* in(static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * size)));
	fftw_complex* out(static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * size)));
	fftw_plan p(fftw_plan_dft_1d(input.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE));
	for (size_t j = 0; j < size; ++j) {
		in[j][0] = input[j];
		in[j][1] = 0;
	}
	fftw_execute(p);
	size /= 2;
	double* power_spectrum = new double[size];
	for (int i = 0; i < size; ++i) {
		power_spectrum[i] = sqrt(powf(out[i][0], 2) + powf(out[i][1], 2));
	}
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);

	mfcc.SetSpectrumData(power_spectrum);
	mfcc.GetLogCoefficents(feature_vec);

	inverseMfcc.SetSpectrumData(power_spectrum);
	inverseMfcc.GetLogCoefficents(feature_vec);
	delete[] power_spectrum;
}

MfccExtractor::MfccExtractor(int feature_size, double window_size) : count(feature_size) {
	mfccs = new double[count];
	windowSize = window_size;
}

MfccExtractor::MfccExtractor(int feature_size, double window_size, int sampling_rate) :
	MfccExtractor(window_size, feature_size) {
	mfcc.Init(feature_size, window_size * sampling_rate, 0, sampling_rate / 2, sampling_rate);
	inverseMfcc.Init(feature_size, window_size * sampling_rate, 0, sampling_rate / 2, sampling_rate);
}

MfccExtractor::~MfccExtractor() {
	delete mfccs;
}
