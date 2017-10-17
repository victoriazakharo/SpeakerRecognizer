#pragma once

#include "export.h"
#include <vector>

class SoundFile;

class FEATURE_API FeatureExtractor {
public:
	virtual ~FeatureExtractor() {
	}

	//sound parameters are not known in advance
	virtual void Extract(std::vector<double>& feature_vec, std::vector<double>& input, SoundFile& sound) = 0;

	//sound parameters are known in advance
	virtual void Extract(std::vector<double>& feature_vec, std::vector<double>& input) = 0;
};

