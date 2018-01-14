#pragma once
#include <string>
#include "FeatureExtractor.h"
#include <armadillo>
#include <memory>
#include <map>

class SpeakerRecognizer {
	double windowSize;
	double hop;
	double (*windowFunc)(int k, int n);
	int featureSize;
	const std::string modelPath;
	const std::string modelFolder;
	std::vector<int> answers;
	std::map<int, std::vector<arma::gmm_diag>> dictors;
	std::vector<std::shared_ptr<FeatureExtractor>> extractors;
	void ExtractFeatures(const std::string& path, std::vector<std::vector<double>>& tests);
	int TellSpeaker(const std::vector<std::vector<double>>& tests) const;
public:
	SpeakerRecognizer(const std::string& model_folder, const std::string& model_file);
	void Initialize();
	int GetFirstResult();
	void SaveResult(const std::string& result_file);
	void Test(const std::string& test_file_name);
	void Test(const std::string& test_file_names, const std::string& folder);
	void TestPreextracted(const std::string& feature_path);
};
