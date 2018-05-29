#pragma once

#include <string>
#include <map>
#include "MfccExtractor.h"

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
	MfccExtractor mfccExtractor;
public:
	SpeakerModeler(const string& model_folder, const string& model_file,
		bool use_imfcc = true);
	void BuildDictorModels(const map<int, map<int, vector<vector<double>>>>& features) const;	
	void ExtractBatchFeatures(const string& folder, const string& alignment_path,
		map<int, map<int, vector<vector<double>>>>& features, map<int, string>& files);
	void ExtractFeatures(const string& folder, const string& align_file_prefix, int record_num,
		map<int, vector<vector<double>>>& features, map<int, string>& files);
	void ReadFeatures(const string& alignment_path, map<int, map<int, vector<vector<double>>>>& features,
	                  const string& feature_path, const string& record_map_path);
};

