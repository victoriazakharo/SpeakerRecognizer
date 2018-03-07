#pragma once
#include "FeatureExtractor.h"
#include "inverse_mfcc.h"
#include <memory>

class FEATURE_API MfccExtractor : public FeatureExtractor {	
	vector<std::shared_ptr<Mfcc>> mfccExtractors;
public:
	//sound parameters are not known in advance
	void Extract(vector<double>& feature_vec, vector<double>& input, SoundFile& sound) override;
	//sound sampling rate is considered to be known in advance
	void Extract(vector<double>& feature_vec, vector<double>& input) override;

	void Init(double window_size, int sampling_rate, bool use_imfcc);
};

