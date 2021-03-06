#include "SpeakerModeler.h"
#include "model_io.h"

using namespace std;

void model_batch(int argc, char* argv[]) {
	if (argc != 7 && argc != 9) {
		printf("Usage: %s <model-folder> <model-file> <alignment-file> <record-folder> <record-location-file> [<kaldi-feature-file> <kaldi-record-map-file>]\n", argv[0]);
		return;
	}
	string model_folder(argv[1]);
	string model_file(argv[2]);
	string alignment_file(argv[3]);
	string record_folder(argv[4]);
	string record_location_file(argv[5]);
	const string use_imfcc(argv[6]);

	// TODO: config
	bool imfcc = use_imfcc == "True" || use_imfcc == "true";
	map<int, map<int, vector<vector<double>>>> features;
	SpeakerModeler processor(model_folder, model_file, imfcc);

	if (argc == 7) {
		map<int, string> file_locations;
		ReadRecordPaths(file_locations, record_location_file);
		processor.ExtractBatchFeatures(record_folder, alignment_file, features, file_locations);
	}
	else {
		string kaldi_feature_file(argv[7]);
		string kaldi_record_map_file(argv[8]);
		processor.ReadFeatures(alignment_file, features, kaldi_feature_file, kaldi_record_map_file);
	}
	processor.BuildDictorModels(features);
}

void model_online(int argc, char* argv[]) {
	if (argc != 4) {
		printf("Usage: %s <user-folder> <number-of-records> <use-imfcc>\n", argv[0]);
		return;
	}
	string user_folder(argv[1]);
	int number_of_records = atoi(argv[2]);
	const string use_imfcc(argv[3]);

	// TODO: config
	bool imfcc = use_imfcc == "True" || use_imfcc == "true";
	string model_folder = user_folder + "models/";
	const string model_file = "model.txt";

	SpeakerModeler processor(model_folder, model_file, imfcc);
	map<int, string> file_locations;
	for(int i = 0; i < number_of_records; i++) {
		file_locations[i] = to_string(i) + "-16k.wav";
	}
	map<int, vector<vector<double>>> dictor_features;
	// TODO: config
	const int dictor_number = 30;
	processor.ExtractFeatures(user_folder, "ali.", number_of_records, dictor_features, file_locations);
	const map<int, map<int, vector<vector<double>>>> features{ { dictor_number, dictor_features } };
	string cmd = "exec rm -rf " + model_folder + "*";
	system(cmd.c_str());
	processor.BuildDictorModels(features);
}

int main(int argc, char* argv[]) {
	model_online(argc, argv);
}