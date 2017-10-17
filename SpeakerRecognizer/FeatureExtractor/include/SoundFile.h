#pragma once

#include "export.h"
#include <vector>
#include <string>

class FEATURE_API SoundFile {
	std::vector<double> data;
	std::string name;
	int samplingRate;
public:
	const std::string& getName() const;
	int getSamplingRate() const;
	std::vector<double>& getData();
	SoundFile();
	bool Initialize(const std::string& file_name);
};

