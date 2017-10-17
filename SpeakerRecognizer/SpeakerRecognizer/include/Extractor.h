#pragma once
#include <string>
#include "FeatureExtractor.h"
#include <armadillo>
#include <memory>
#include <map>

class Extractor {
	double windowSize;
	double hop;
	unsigned gaussians;
	double (*windowFunc)(const int k, const int n);
	int featureSize;
	const std::string modelPath;
	const std::string modelFolder;
	const std::string resultFile;
	std::vector<int> answers;
	std::map<int, std::vector<arma::gmm_diag>> dictors;
	std::vector<std::shared_ptr<FeatureExtractor>> extractors;
	void ExtractFeatures(const std::string& path, std::vector<std::vector<double>>& tests);
	int TellSpeaker(const std::vector<std::vector<double>>& tests) const;
public:
	Extractor(const std::string& model_folder, const std::string& model_file, const std::string& result_file);
	void Initialize();
	void SaveResult();
	void Test(const std::string& test_file_names, const std::string& folder);
	void TestPreextracted(const std::string& feature_path);
};
