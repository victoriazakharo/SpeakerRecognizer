#pragma once

#include <string>
#include <map>
#include <vector>
#include <armadillo>

std::map<int, std::vector<arma::gmm_diag>> ReadModel(const std::string& folder,
	const std::string& file_name);

void SaveAnswers(const std::string& result_file, std::vector<int> &answers);
