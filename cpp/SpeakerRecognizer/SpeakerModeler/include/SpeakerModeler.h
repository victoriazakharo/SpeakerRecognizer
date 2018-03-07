#pragma once

#include <string>
#include <memory>
#include <map>
#include "FeatureExtractor.h"

using std::vector;
using std::map;
using std::string;

class SpeakerModeler {
	double windowSize;
	double hop;
	unsigned gaussians;
	double (*windowFunc)(int k, int n);
	int featureSize;
	const string modelPath;
	const string modelFolder;
	vector<std::shared_ptr<FeatureExtractor>> extractors;
public:
	SpeakerModeler(const string& model_folder, const string& model_file,
		bool use_imfcc = true);
	void BuildDictorModels(const map<int, map<int, vector<vector<double>>>>& features) const;
	void ExtractFeatures(const string& folder, const string& alignment_path,
	                     map<int, map<int, vector<vector<double>>>>& features, const string& file_path);
	void ReadFeatures(const string& alignment_path, map<int, map<int, vector<vector<double>>>>& features,
	                  const string& feature_path, const string& record_map_path);
};

