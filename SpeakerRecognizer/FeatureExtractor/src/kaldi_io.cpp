#include "kaldi_io.h"
#include <fstream>
#include <sstream>

using std::vector;
using std::map;
using std::string;

void FillRecordMap(map<string, int>& files, const string& file_path) {
	string line;
	string path;
	int id;
	std::ifstream fi(file_path);
	if (fi.is_open()) {
		while (getline(fi, line)) {
			std::stringstream stream(line);
			stream >> path >> id;
			files[path] = id;
		}
	}
	else {
		printf("Cannot open file %s\n", file_path.c_str());
	}
	fi.close();
}

void ReadKaldiFeatures(map<int, vector<vector<double>>>& features, const string& path, const string& record_map_path) {
	map<string, int> files;
	FillRecordMap(files, record_map_path);
	string record, dictor, new_dictor, line;
	std::ifstream fi(path);
	if (getline(fi, line)) {
		while (!fi.eof() && (line.length() < 20 || getline(fi, line))) {
			record = line.substr(0, line.find_first_of(' '));
			while (getline(fi, line) && line.length() > 20) {
				std::stringstream stream(line);
				vector<double> window;
				double n;
				while (stream >> n) {
					window.push_back(n);
				}
				features[files[record]].push_back(window);
			}
		}
	}
	fi.close();
}

void ReadKaldiFeatures(vector<vector<vector<double>>>& features, const string& path) {
	string record, dictor, new_dictor, line;
	std::ifstream fi(path);
	if (getline(fi, line)) {
		while (!fi.eof() && (line.length() < 20 || getline(fi, line))) {
			vector<vector<double>> rec_features;
			while (getline(fi, line) && line.length() > 20) {
				std::stringstream stream(line);
				vector<double> window;
				double n;
				while (stream >> n) {
					window.push_back(n);
				}
				rec_features.push_back(window);
			}
			features.push_back(rec_features);
		}
	}
	fi.close();
}