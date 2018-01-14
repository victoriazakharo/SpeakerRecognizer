#define _USE_MATH_DEFINES

#include <fstream>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include <math.h>
#include "mfcc.h"

Mfcc::Mfcc() {
	this->data = nullptr;
}

void Mfcc::Init(int noFilterBanks, int NFFT, double minFreq, double maxFreq, double sampleFreq) {
	this->noFilterBanks = noFilterBanks;
	this->NFFT = NFFT;
	this->minFreq = minFreq;
	this->maxFreq = maxFreq;
	this->sampleFreq = sampleFreq;
	InitFilterBanks();
}

void Mfcc::InitFilterBanks() {
	filterBanks.clear();

	double minMel = MelScale(minFreq);
	double maxMel = MelScale(maxFreq);
	double dMel = (maxMel - minMel) / (noFilterBanks + 1);
	vector<double> melSpacing;
	vector<double> fftFreqs2Mel;

	// Init melSpacing
	for (int i = 0; i < noFilterBanks + 2; i++) {
		double mel = minMel + i * dMel;
		melSpacing.push_back(mel);
	}	
	
	double interval = sampleFreq / NFFT;

	int bins_num = NFFT / 2;
	// Init fftFreqs2Mel
	for (int i = 0; i < bins_num; i++) {
		double fftFreq2Mel = MelScale(i * interval);
		fftFreqs2Mel.push_back(fftFreq2Mel);
	}

	// Prepare the mel scale filterbank
	for (int i = 0; i < noFilterBanks; i++) {
		vector<double> fBank;
		for (int j = 0; j < bins_num; j++) {
			double val = std::max(0.0, 1.0 - abs(fftFreqs2Mel[j] - melSpacing[i + 1]) / (melSpacing[i + 1] - melSpacing[i]));
			fBank.push_back(val);
		}
		filterBanks.push_back(fBank);
	}
}

void Mfcc::SetSpectrumData(double* data) {
	this->data = data;
}

void Mfcc::GetLogCoefficents(vector<double>& v) {

	if (this->data == nullptr) {
		printf("No Data!\n");
		exit(-1);
	}
	// Initialize pre-discrete cosine transformation vector array
	vector<double> preDCT; 	
	// Map the spectrum to the mel scale (apply triangular filters)
	// For each filter bank (i.e. for each mel frequency)
	for (auto& it : filterBanks) {
		double cel = 0;
		int n = 0;
		// For each frequency in the original spectrum
		for (auto& it2 : it) {
			cel += it2 * data[n++];
		}
		// Compute the log of the mel-frequency spectrum
		preDCT.push_back(log10(cel));
	}

	// Perform the Discrete Cosine Transformation
	int num_filters = filterBanks.size();

	double mul = M_PI / num_filters;
	double norm_factor = sqrt(2.0 / num_filters);

	double val = 0;
	for (int j = 0; j < num_filters; j++) {
		val += preDCT[j];
	}
	v.push_back(val / sqrt(2.0) * norm_factor);
	int num_coefs = num_filters;
	for (int i = 1; i < num_coefs; i++) {
		val = 0;
		for (int j = 0; j < num_filters; j++) {
			val += preDCT[j] * cos(i * (j + 0.5) * mul);
		}
		v.push_back(val * norm_factor);
	}
}

double Mfcc::MelScale(double freq) const {
	return 2595 * log(1.0 + freq / 700.0);
}
