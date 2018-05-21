#include "SpeakerModeler.h"
#include "model_io.h"

using namespace std;

void model_batch(int argc, char* argv[]) {
	if (argc != 6 && argc != 8) {
		printf("Usage: %s <model-folder> <model-file> <alignment-file> <record-folder> <record-location-file> [<kaldi-feature-file> <kaldi-record-map-file>]\n", argv[0]);
		return;
	}
	string model_folder(argv[1]);
	string model_file(argv[2]);
	string alignment_file(argv[3]);
	string record_folder(argv[4]);
	string record_location_file(argv[5]);

	map<int, map<int, vector<vector<double>>>> features;
	SpeakerModeler processor(model_folder, model_file);

	if (argc == 6) {
		map<int, string> file_locations;
		ReadRecordPaths(file_locations, record_location_file);
		processor.ExtractFeatures(record_folder, alignment_file, features, file_locations);
	}
	else {
		string kaldi_feature_file(argv[6]);
		string kaldi_record_map_file(argv[7]);
		processor.ReadFeatures(alignment_file, features, kaldi_feature_file, kaldi_record_map_file);
	}
	processor.BuildDictorModels(features);
}

void model_online(int argc, char* argv[]) {
	if (argc != 3) {
		printf("Usage: %s <user-folder> <number-of-records>\n", argv[0]);
		return;
	} 
	string user_folder(argv[1]);
	int number_of_records = atoi(argv[2]);
	string model_folder = user_folder + "models/";
	const string model_file = "model.txt";
	string alignment_file = user_folder + "result_ali.txt";
	

	map<int, map<int, vector<vector<double>>>> features;
	SpeakerModeler processor(model_folder, model_file);
	map<int, string> file_locations;
	for(int i = 0; i < number_of_records; i++) {
		file_locations[i] = to_string(i) + "-16k.wav";
	}
	processor.ExtractFeatures(user_folder, alignment_file, features, file_locations);
	processor.BuildDictorModels(features);
}

int main(int argc, char* argv[]) {
	model_online(argc, argv);
}