#pragma once

#include <string>
#include <map>
#include <armadillo>

using std::map;
using std::string;

void WriteModel(const string& folder, const string& file_name,
	map<int, map<int, arma::gmm_diag>>& data);

void ReadRecordPaths(map<int, string>& files, const string& file_path);