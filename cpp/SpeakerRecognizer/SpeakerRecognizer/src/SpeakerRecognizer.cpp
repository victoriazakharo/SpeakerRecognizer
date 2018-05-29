#include "SpeakerRecognizer.h"
#include "SoundFile.h"
#include "window.inl"
#include "MfccExtractor.h"
#include "model_io.h"
#include <cfloat>
#include <kaldi_io.h>

using namespace arma;

void SpeakerRecognizer::ExtractFeatures(const string& path,
	vector<vector<double>>& tests) {

	SoundFile sound;
	if (!sound.Initialize(path)) {
		fprintf(stderr, "Cannot parse file %s\n", path.c_str());
		return;
	}
	int sampling_rate = sound.getSamplingRate();
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
		mfccExtractor.Extract(segment_result, input);
		tests.push_back(segment_result);
	}
}

int best_speaker(map<int, int>&result) {
	int max = 0;
	int best = 0;
	for (auto& kv : result) {
		if (kv.second > max) {
			max = kv.second;
			best = kv.first;
		}
	}
	return best;
}

int SpeakerRecognizer::TellSpeaker(const vector<vector<double>>& tests) const {	
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
	return best_speaker(result);
}

int SpeakerRecognizer::TellSpeaker(const vector<vector<double>>& tests, int dictor, vector<gmm_diag>& dictor_phonemes) const {
	int best;
	map<int, int> result;
	for (auto& kv : dictors) {
		result[kv.first] = 0;
	}
	for (int i = 0; i < tests.size(); i++) {
		double cur, max = -DBL_MAX;
		mat F(tests[i]);
		for (auto& kv : dictors) {
			for (auto& model : kv.second) {
				cur = model.avg_log_p(F);
				if (cur > max) {
					max = cur;
					best = kv.first;
				}
			}
		}
		for (auto& model : dictor_phonemes) {
			cur = model.avg_log_p(F);
			if (cur > max) {
				max = cur;
				best = dictor;
			}
		} 
		result[best]++;
	}
	return best_speaker(result);
}

int SpeakerRecognizer::Test(const string& test_file_name) {
	vector<vector<double>> features;
	ExtractFeatures(test_file_name, features);
	return TellSpeaker(features);
}

int SpeakerRecognizer::Test(const string& test_file_name, const string& dictor_folder) {
	map<int, std::vector<gmm_diag>> dictor_map(ReadModel(dictor_folder, dictor_folder + modelFile));
	vector<vector<double>> features;
	ExtractFeatures(test_file_name, features);
	if(dictor_map.empty()) {
		return TellSpeaker(features);
	}
	auto dictor_pair = dictor_map.begin();
	return TellSpeaker(features, dictor_pair->first, dictor_pair->second);
}


vector<int> SpeakerRecognizer::TestBatch(const string& test_file_names, const string& folder) {
	vector<int> answers;
	int dictor;
	string line, record;
	std::ifstream f(test_file_names);
	if (f.is_open()) {
		while (getline(f, line)) {
			std::stringstream stream(line);
			stream >> dictor >> record;
			vector<vector<double>> features;
			ExtractFeatures(folder + record, features);
			int answer = TellSpeaker(features);
			printf("%d\n", answer);
			answers.push_back(answer);
		}
	}
	f.close();
	return answers;
}

vector<int> SpeakerRecognizer::TestPreextracted(const string& feature_path) {
	vector<int> answers;
	vector<vector<vector<double>>> features;
	ReadKaldiFeatures(features, feature_path);
	for (int i = 0; i < features.size(); i++) {
		answers.push_back(TellSpeaker(features[i]));
	}
	return answers;
}

SpeakerRecognizer::SpeakerRecognizer(const string& model_folder,
	const string& model_file, bool use_imfcc) :
	// TODO: move to config file
	windowSize(0.025),
	hop(0.010),
	windowFunc(HammingWindow),
	modelFile(model_file),
	modelFolder(model_folder),
	dictors(ReadModel(modelFolder, model_folder +  model_file)) {
	//TODO: ensure fixed sample rate
	mfccExtractor.Init(windowSize, 16000, use_imfcc);
}