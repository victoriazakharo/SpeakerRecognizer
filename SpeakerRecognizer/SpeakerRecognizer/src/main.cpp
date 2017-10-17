#include "Extractor.h"
using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 6 && argc != 7) {
		printf("Usage: %s <result-file> <model-folder> <model-file> <expected-result-file> <record-folder> [<kaldi-feature-file>]\n", argv[0]);
		return 0;
	}

	string result_file(argv[1]);
	string model_folder(argv[2]);
	string model_file(argv[3]);
	string expected_result_file(argv[4]);
	string record_folder(argv[5]);

	Extractor p(model_folder, model_file, result_file);
	p.Initialize();

	if (argc == 6) {
		p.Test(expected_result_file, record_folder);
	}
	else {
		string kaldi_feature_file(argv[6]);
		p.TestPreextracted(kaldi_feature_file);
	}
	p.SaveResult();
	return 0;
}
