#pragma once

#include <string>
#include <map>
#include <vector>
#include <armadillo>

void ReadModel(const std::string& folder, const std::string& file_name,
	std::map<int, std::vector<arma::gmm_diag>>& data);
