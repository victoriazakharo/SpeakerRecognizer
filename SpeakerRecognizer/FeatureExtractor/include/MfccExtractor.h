#pragma once
#include "FeatureExtractor.h"
#include "inverse_mfcc.h"

class FEATURE_API MfccExtractor : public FeatureExtractor {
	int count;
	int windowSize;
	double* mfccs;

	Mfcc mfcc;
	InverseMfcc inverseMfcc;
public:
	//sound parameters are not known in advance
	void Extract(vector<double>& feature_vec, vector<double>& input, SoundFile& sound) override;
	//sound parameters are known in advance
	void Extract(vector<double>& feature_vec, vector<double>& input) override;

	MfccExtractor(int feature_size, double window_size);
	MfccExtractor(int feature_size, double window_size, int sampling_rate);
	~MfccExtractor();
};

