#pragma once
#include <string>
#include <armadillo>
#include <map>
#include "MfccExtractor.h"

using std::vector;
using std::string;
using std::map;

class SpeakerRecognizer {
	const double windowSize;
	const double hop;
	double (*windowFunc)(int k, int n);
	const string modelFile;
	const string modelFolder;

	const map<int, vector<arma::gmm_diag>> dictors;
	MfccExtractor mfccExtractor;
	void ExtractFeatures(const string& path, vector<vector<double>>& tests);
	int TellSpeaker(const vector<vector<double>>& tests) const;
	int TellSpeaker(const vector<vector<double>>& tests, int dictor, vector<arma::gmm_diag>& dictor_phonemes) const;
public:
	SpeakerRecognizer(const string& model_folder, const string& model_file,
		bool use_imfcc = true);
	int Test(const string& test_file_name);
	int Test(const string& test_file_name, const string &dictor_folder);
	vector<int> TestBatch(const string& test_file_names, const string& folder);
	vector<int> TestPreextracted(const string& feature_path);
};
