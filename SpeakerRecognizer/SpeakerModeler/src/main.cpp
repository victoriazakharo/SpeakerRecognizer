#include "SpeakerModeler.h"

using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 6 && argc != 8) {
		printf("Usage: %s <model-folder> <model-file> <alignment-file> <record-folder> <record-location-file> [<kaldi-feature-file> <kaldi-record-map-file>]\n", argv[0]);
		return 0;
	}
	string model_folder(argv[1]);
	string model_file(argv[2]);
	string alignment_file(argv[3]);
	string record_folder(argv[4]);
	string record_location_file(argv[5]);

	map<int, map<int, vector<vector<double>>>> features;
	SpeakerModeler processor(model_folder, model_file);

	if (argc == 6) {
		processor.ExtractFeatures(record_folder, alignment_file, features, record_location_file);
	}
	else {
		string kaldi_feature_file(argv[6]);
		string kaldi_record_map_file(argv[7]);
		processor.ReadFeatures(alignment_file, features, kaldi_feature_file, kaldi_record_map_file);
	}
	processor.BuildDictorModels(features);
	return 0;
}

