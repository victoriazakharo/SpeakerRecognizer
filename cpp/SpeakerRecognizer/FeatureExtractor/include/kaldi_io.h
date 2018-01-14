#pragma once

#include <string>
#include <map>
#include <vector>
#include "export.h"

using std::vector;
using std::string;
using std::map;

FEATURE_API void ReadKaldiFeatures(map<int, vector<vector<double>>>& features,
	const string& path, const string& record_map_path);
FEATURE_API void ReadKaldiFeatures(vector<vector<vector<double>>>& features,
	const string& path);