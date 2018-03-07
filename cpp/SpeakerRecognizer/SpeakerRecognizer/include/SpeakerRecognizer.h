#pragma once
#include <string>
#include <armadillo>
#include <memory>
#include <map>
#include "MfccExtractor.h"

using std::vector;
using std::string;
using std::map;

class SpeakerRecognizer {
	const double windowSize;
	const double hop;
	double (*windowFunc)(int k, int n);
	const string modelPath;
	const string modelFolder;
	const map<int, vector<arma::gmm_diag>> dictors;
	MfccExtractor mfccExtractor;
	void ExtractFeatures(const string& path, vector<vector<double>>& tests);
	int TellSpeaker(const vector<vector<double>>& tests) const;
public:
	SpeakerRecognizer(const string& model_folder, const string& model_file,
		bool use_imfcc = true);
	int Test(const string& test_file_name);
	vector<int> Test(const string& test_file_names, const string& folder);
	vector<int> TestPreextracted(const string& feature_path);
};
