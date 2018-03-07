#include "SpeakerModeler.h"
#include "kaldi_io.h"
#include "SoundFile.h"
#include "MfccExtractor.h"
#include "model_io.h"
#include "window.inl"

#ifdef _MSC_VER 
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif

using namespace arma;

void ReadRecordPaths(map<int, string>& files, const string& file_path) {
	string line, path;
	int id;
	std::ifstream fi(file_path);
	if (fi.is_open()) {
		while (getline(fi, line)) {
			std::stringstream stream(line);
			stream >> id >> path;
			files[id] = path;
		}
	}
	else {
		fprintf(stderr, "Cannot open file %s\n", file_path.c_str());
	}
	fi.close();
}

void SpeakerModeler::BuildDictorModels(const map<int, map<int, vector<vector<double>>>>& features) const {
	map<int, map<int, gmm_diag>> models;
	for (auto& kv : features) {
		for (auto& fkv : kv.second) {
			if (fkv.second.size() >= gaussians) {
				gmm_diag model;
				mat F(fkv.second);
				if (model.learn(F, gaussians, maha_dist, random_subset, 10, 100, 1e-4, false)) {
					models[kv.first][fkv.first] = model;
				}
			}
		}
	}
	WriteModel(modelFolder, modelPath, models);
}

void SpeakerModeler::ExtractFeatures(const string& folder, const string& alignment_path, 
	map<int, map<int, vector<vector<double>>>>& features, const string& file_path) {

	map<int, string> files;
	ReadRecordPaths(files, file_path);
	string line;
	int phoneme, record, dictor, prev_record = -1;
	int window_size_in_samples, hop_in_samples, sampling_rate, offset;
	double start, end;
	SoundFile sound;
	vector<double>& data = sound.getData();
	std::ifstream f(alignment_path);
	if (f.is_open()) {
		while (getline(f, line)) {
			SSCANF(&line[0], "%d %d %lf %lf %d", &dictor, &record, &start, &end, &phoneme);
			if (prev_record != record) {
				if (!sound.Initialize(folder + files[record])) {
					printf("Cannot parse file %s\n", (folder + files[record]).c_str());
					return;
				}
				sampling_rate = sound.getSamplingRate();
				window_size_in_samples = windowSize * sampling_rate;
				offset = window_size_in_samples / 4;
				hop_in_samples = hop * sampling_rate;
				data = sound.getData();
				prev_record = record;
				printf("%d\n", record);
			}
			int start_in_samples = start * sampling_rate - offset;
			start_in_samples = std::max(start_in_samples, 0);
			int end_in_samples = end * sampling_rate + offset;
			end_in_samples = std::min(end_in_samples, static_cast<int>(data.size())) - window_size_in_samples;
			for (int i = start_in_samples; i < end_in_samples; i += hop_in_samples) {
				vector<double> input;
				input.reserve(window_size_in_samples);
				for (int j = 0; j < window_size_in_samples; ++j) {
					input.push_back(data[i + j] * windowFunc(j, window_size_in_samples));
				}
				vector<double> segment_result;
				for (const auto& extractor : extractors) {
					extractor->Extract(segment_result, input);
				}
				features[dictor][phoneme].push_back(segment_result);
			}
		}
		f.close();
	}
	else {
		fprintf(stderr, "Cannot open file %s\n", alignment_path.c_str());
	}
}

void SpeakerModeler::ReadFeatures(const string& alignment_path, 
	map<int, map<int, vector<vector<double>>>>& features,
	const string& feature_path, const string& record_map_path) {

	string line;
	int phoneme, record, dictor;
	int start_in_samples, end_in_samples;
	double start, end;
	map<int, vector<vector<double>>> trains;
	map<string, int> record_map;
	ReadKaldiFeatures(trains, feature_path, record_map_path);
	std::ifstream f(alignment_path);
	if (f.is_open()) {
		while (getline(f, line)) {
			SSCANF(&line[0], "%d %d %lf %lf %d", &dictor, &record, &start, &end, &phoneme);
			start_in_samples = start / windowSize;
			end_in_samples = end / windowSize;
			for (int i = start_in_samples; i <= end_in_samples; i++) {
				features[dictor][phoneme].push_back(trains[record][i]);
			}
		}
		f.close();
	}
	else {
		fprintf(stderr, "Cannot open file %s\n", alignment_path.c_str());
	}
}

SpeakerModeler::SpeakerModeler(const string& model_folder,
	const string& model_file, bool use_imfcc) :
	// TODO: move to config file
	windowSize(0.025), hop(0.01), gaussians(3), windowFunc(HammingWindow),
	featureSize(13), modelPath(model_folder + model_file), modelFolder(model_folder) {

	auto ptr = std::make_shared<MfccExtractor>();

	//TODO: ensure fixed sample rate 
	ptr->Init(windowSize, 16000, use_imfcc);	
	extractors.push_back(ptr);
}