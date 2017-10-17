#include "Extractor.h"
#include "SoundFile.h"
#include "window.inl"
#include "MfccExtractor.h"
#include "model_io.h"
#include <float.h>
#include <kaldi_io.h>

using namespace arma;

void Extractor::ExtractFeatures(const string& path, vector<vector<double>>& tests) {
	SoundFile sound;
	if (!sound.Initialize(path)) {
		printf("Cannot parse file %s\n", path.c_str());
		return;
	}
	double sampling_rate = sound.getSamplingRate();
	double window_size_in_samples = windowSize * sampling_rate;
	double hop_in_samples = hop * sampling_rate;
	const vector<double>& data = sound.getData();
	int end = data.size() - window_size_in_samples;
	for (int i = 0; i < end; i += hop_in_samples) {
		vector<double> input;
		for (int j = 0; j < window_size_in_samples; ++j) {
			input.push_back(data[i + j] * windowFunc(j, window_size_in_samples));
		}
		vector<double> segment_result;
		for (const auto& extractor : extractors) {
			extractor->Extract(segment_result, input);
		}
		tests.push_back(segment_result);
	}
}

int Extractor::TellSpeaker(const vector<vector<double>>& tests) const {
	int best;
	map<int, int> result;
	for (auto& kv : dictors) {
		result[kv.first] = 0;
	}
	for (int i = 0; i < tests.size(); i++) {
		double cur, max = -DBL_MAX;
		for (auto& kv : dictors) {
			for (auto& model : kv.second) {
				mat F(tests[i]);
				cur = model.avg_log_p(F);
				if (cur > max) {
					max = cur;
					best = kv.first;
				}
			}
		}
		result[best]++;
	}
	int max = 0;
	for (auto& kv : result) {
		if (kv.second > max) {
			max = kv.second;
			best = kv.first;
		}
	}
	printf("%d\n", best);
	return best;
}

void Extractor::Test(const string& test_file_names, const string& folder) {
	answers.clear();
	int dictor;
	string line, record;
	std::ifstream f(test_file_names);
	if (f.is_open()) {
		while (getline(f, line)) {
			std::stringstream stream(line);
			stream >> dictor >> record;
			vector<vector<double>> features;

			ExtractFeatures(folder + record, features);
			answers.push_back(TellSpeaker(features));
		}
	}
	f.close();
}

void Extractor::TestPreextracted(const string& feature_path) {
	answers.clear();
	vector<vector<vector<double>>> features;
	ReadKaldiFeatures(features, feature_path);
	for (int i = 0; i < features.size(); i++) {
		answers.push_back(TellSpeaker(features[i]));
	}
}


void Extractor::Initialize() {
	dictors.clear();
	ReadModel(modelFolder, modelPath, dictors);
}

void Extractor::SaveResult() {
	std::ofstream out(resultFile, ios::binary);
	for (int i = 0; i < answers.size(); i++) {
		out << answers[i] << "\n";
	}
	out.close();
}

Extractor::Extractor(const string& model_folder, const string& model_file, const string& result_file) : windowSize(0.025),
                                                                                                        hop(0.010), gaussians(3), windowFunc(HammingWindow), featureSize(13), modelPath(model_folder + model_file),
                                                                                                        modelFolder(model_folder), resultFile(result_file) {
	std::shared_ptr<FeatureExtractor> extractor_ptr;
	extractor_ptr.reset(new MfccExtractor(featureSize, windowSize, 16000));
	extractors.push_back(extractor_ptr);
}