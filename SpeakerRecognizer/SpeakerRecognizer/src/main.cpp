#include "SpeakerRecognizer.h"

#include "socket.h"
#include "stdio.h"
#include <string>

using namespace std;

void recognize_batch(int argc, char* argv[]) {
	if (argc != 6 && argc != 7) {
		printf("Usage: %s <result-file> <model-folder> <model-file> "
			"<expected-result-file> <record-folder> [<kaldi-feature-file>]\n", argv[0]);
		return;
	}
	string result_file(argv[1]);
	string model_folder(argv[2]);
	string model_file(argv[3]);
	string expected_result_file(argv[4]);
	string record_folder(argv[5]);

	SpeakerRecognizer recognizer(model_folder, model_file);
	recognizer.Initialize();

	if (argc == 6) {
		recognizer.Test(expected_result_file, record_folder);
	}
	else {
		string kaldi_feature_file(argv[6]);
		recognizer.TestPreextracted(kaldi_feature_file);
	}
	recognizer.SaveResult(result_file);
}

void recognize_online(int argc, char* argv[]) {
	if (argc != 3) {
		printf("Usage: %s <model-folder> <model-file>\n", argv[0]);
		return;
	}
	string model_folder(argv[1]);
	string model_file(argv[2]);

	SpeakerRecognizer recognizer(model_folder, model_file);
	recognizer.Initialize();

	SOCKET socket;
	init_socket(socket);
	printf("Initialized socket...\n");	
	char c;
	string message;
	do {
		const int received = recv(socket, &c, 1, 0);
		if (received > 0) {
			if (c == '\n') {
				recognizer.Test(message);
				string answer = to_string(recognizer.GetFirstResult()) + "\n";
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