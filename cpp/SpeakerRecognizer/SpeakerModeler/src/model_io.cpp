#include "model_io.h"
using std::string;

void WriteModel(const string& folder, const string& file_name, std::map<int, std::map<int, arma::gmm_diag>>& data) {
	std::vector<int> model_sizes;
	std::vector<int> dictors;
	std::ifstream in(file_name, std::ios::binary);
	int tmp;
	int dictors_num = 0;
	if (in >> dictors_num) {
		for (int i = 0; i < dictors_num; i++) {
			in >> tmp;
			dictors.push_back(tmp);
			in >> tmp;
			model_sizes.push_back(tmp);
		}
	}
	in.close();
	std::ofstream out(file_name, std::ios::binary);
	out << data.size() + dictors_num << " ";
	for (int i = 0; i < dictors_num; i++) {
		out << dictors[i] << " " << model_sizes[i] << " ";
	}
	for (auto& kv : data) {
		int count = 0;
		out << kv.first << " ";
		out << kv.second.size() << " ";
		for (auto& fkv : kv.second) {
			fkv.second.save(folder + std::to_string(kv.first) + "." + std::to_string(count++));
		}
	}
	out << "\n";
	out.close();
}
