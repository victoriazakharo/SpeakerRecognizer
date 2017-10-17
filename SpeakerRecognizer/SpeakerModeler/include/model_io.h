#pragma once

#include <string>
#include <map>
#include <armadillo>

void WriteModel(const std::string& folder, const std::string& file_name, std::map<int, std::map<int, arma::gmm_diag>>& data);