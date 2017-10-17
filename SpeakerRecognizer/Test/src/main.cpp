#include <string>
#include <fstream>
#include <sstream>

using std::string;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("Usage: %s <ideal-file> <test-file>\n", argv[0]);
		return 0;
	}
	string ideal(argv[1]);
	string test(argv[2]);
	std::ifstream ideal_file(ideal, std::ios::binary);
	std::ifstream test_file(test, std::ios::binary);
	int right = 0, total = 0;
	int right_answer, test_answer;
	string ideal_line, test_line;
	if (ideal_file.is_open()) {
		if (test_file.is_open()) {
			while (getline(ideal_file, ideal_line) && getline(test_file, test_line)) {
				std::stringstream ideal_stream(ideal_line);
				ideal_stream >> right_answer;
				std::stringstream test_stream(test_line);
				test_stream >> test_answer;
				if (test_answer == right_answer) {
					right++;
				}
				else {
					printf("  best: %d\tright: %d\n", test_answer, right_answer);
				}
				total++;
			}
		}
		else {
			printf("Cannot open file %s\n", test.c_str());
			ideal_file.close();
			return 0;
		}
	}
	else {
		printf("Cannot open file %s\n", ideal.c_str());
		return 0;
	}
	ideal_file.close();
	test_file.close();
	printf("Right records: %d/%d\n", right, total);
	return 0;
}

