#include "model_io.h"

std::map<int, std::vector<arma::gmm_diag>> ReadModel(const std::string& folder,
	const std::string& file_name) {

	std::map<int, std::vector<arma::gmm_diag>> result;
	try {
		std::ifstream in(file_name, std::ios::binary);
		if(in.fail()) {
			return result;
		}
		std::vector<int> model_sizes;
		std::vector<int> dictors;
		printf("Reading models from %s\n", folder.c_str());
		int tmp;
		int dictors_num;
		in >> dictors_num;
		for (int i = 0; i < dictors_num; i++) {
			in >> tmp;
			dictors.push_back(tmp);
			in >> tmp;
			model_sizes.push_back(tmp);
		}
		for (int i = 0; i < dictors_num; i++) {
			for (int j = 0; j < model_sizes[i]; j++) {
				arma::gmm_diag model;
				model.load(folder + std::to_string(dictors[i]) + "." + std::to_string(j));
				result[dictors[i]].push_back(model);
			}
			printf("\r%d of %d", i, dictors_num);
		}
		printf("...Finished reading\n");
		in.close();
	}
	catch(...) {
		fprintf(stderr, "...Error reading models!\n");
	}
	return result;
}

void SaveAnswers(const std::string& result_file, std::vector<int> &answers) {
	std::ofstream out(result_file);
	for (auto answer : answers) {
		out << answer << "\n";
	}
	out.close();
}
