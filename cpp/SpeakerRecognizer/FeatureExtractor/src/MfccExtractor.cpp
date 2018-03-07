#include <fftw3.h>
#include <math.h>
#include "MfccExtractor.h"
#include "SoundFile.h"
#include "inverse_mfcc.h"
#include <stdexcept>

using std::vector;

void MfccExtractor::Extract(vector<double>& feature_vec, vector<double>& input, SoundFile& sound) {
	throw std::logic_error{ "Not implemented." };
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

	for (int i = 0; i < mfccExtractors.size(); i++) {
		mfccExtractors[i]->GetLogCoefficents(power_spectrum, feature_vec);
	}
	delete[] power_spectrum;
}

void MfccExtractor::Init(double window_size, int sampling_rate, bool use_imfcc) {
	const int feature_size = 13;
	mfccExtractors.push_back(std::make_shared<Mfcc>());
	if (use_imfcc) {
		mfccExtractors.push_back(std::make_shared<InverseMfcc>());
	}
	for (int i = 0; i < mfccExtractors.size(); i++) {
		mfccExtractors[i]->Init(feature_size, window_size, sampling_rate);
	}
}
