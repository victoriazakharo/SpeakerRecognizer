#include "SpeakerRecognizer.h"

#include "socket.h"
#include "stdio.h"
#include <string>
#include "model_io.h"

using namespace std;

void recognize_batch(int argc, char* argv[]) {
	if (argc != 7 && argc != 8) {
		printf("Usage: %s <result-file> <model-folder> <model-file> "
			"<expected-result-file> <record-folder> <use-imfcc>"
			" [<kaldi-feature-file>]\n", argv[0]);
		return;
	}
	const string result_file(argv[1]);
	const string model_folder(argv[2]);
	const string model_file(argv[3]);
	const string expected_result_file(argv[4]);
	const string record_folder(argv[5]);
	const string use_imfcc(argv[6]);

	// TODO: config
	bool mfcc = use_imfcc == "True" || use_imfcc == "true";
	SpeakerRecognizer recognizer(model_folder, model_file, mfcc);

	vector<int> answers;
	if (argc == 7) {
		answers = recognizer.TestBatch(expected_result_file, record_folder);
	}
	else {
		const string kaldi_feature_file(argv[7]);
		answers = recognizer.TestPreextracted(kaldi_feature_file);
	}
	SaveAnswers(result_file, answers);
}

void recognize_online(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: %s <base-folder>\n", argv[0]);
		return;
	}
	const string base_folder(argv[1]);
	const string model_file = "model.txt";
	const string model_folder = "/models/";
	const string english_prefix = "en_";

	// TODO: config sources
	vector<string> sources{ "en_Timit", "en_LibriSpeech", "be_BelarusianSpeech" };
	map<string, SpeakerRecognizer> recognizers;
	for(auto & source : sources) {
		const string model_path = base_folder + source + model_folder;
		const bool use_imfcc = !source.compare(0, english_prefix.size(), english_prefix);
		recognizers.emplace(source, SpeakerRecognizer(model_path, model_file, use_imfcc));
	}

	SOCKET socket;
	init_socket(socket);
	printf("Initialized socket...\n");	
	char c;
	string message;
	// WARN: path shouldn't contain ' ', '\n'
	do {
		const int received = recv(socket, &c, 1, 0);
		if (received > 0) {
			if (c == '\n') {
				size_t space_ix = message.find(' ');
				string source = message.substr(0, space_ix);
				string file = message.substr(space_ix + 1);
				space_ix = file.find(' ');
				string answer;
				// TODO: make smth for checking if ask to recoginze (if ".wav")
				if (file.back() == 'v') {
					const string dictor_folder = file.substr(0, space_ix) + "models/";
					file = file.substr(space_ix + 1);
					// TODO: handle not found case
					auto it = recognizers.find(source);
					const int dictor = it != recognizers.end() ? it->second.Test(file, dictor_folder) : -1;
					answer = to_string(dictor) + "\n";
				}
				else {
					string cmd = "./SpeakerModeler " + file;
					system(cmd.c_str());
					answer = "ok\n";
				}
				message.clear();
				if (send(socket, &answer[0], answer.size(), 0) < 0) {
					handle_socket_error(socket);
				}
			}
			else {
				message += c;
			}
		}
		else if (received == 0) {
			printf("Connection closing...\n");
			break;
		}
		else {
			handle_socket_error(socket);
		}
	} while (true);
	shutdown_socket(socket);
}

int main(int argc, char* argv[]) {
	recognize_online(argc, argv);
}
