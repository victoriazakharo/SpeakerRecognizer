#include "SoundFile.h"
#include "sndfile.h"
#include <algorithm>
#include <math.h>
#include <stdexcept>
#include <vector>
#include "mpg123.h"
#include <iterator>

const std::string& SoundFile::getName() const {
	return name;
}

int SoundFile::getSamplingRate() const {
	return samplingRate;
}

std::vector<double>& SoundFile::getData() {
	return data;
}

SoundFile::SoundFile() : samplingRate(44100) {
}

template <class in_it, class out_it>
out_it copy_every_n(in_it b, in_it e, out_it r, size_t n) {
	for (size_t i = std::distance(b, e) / n; i--; std::advance(b, n))
		*r++ = *b;
	return r;
}

bool SoundFile::Initialize(const std::string& file_name) {
	if (file_name.empty()) {
		printf("Cannot initialize file with empty name\n");
		return false;
	}
	name = file_name;
	SF_INFO info = {};
	
	SNDFILE* file = sf_open(file_name.c_str(), SFM_READ, &info);
	if (file) {
		data.clear();
		samplingRate = info.samplerate;
		data.resize(info.channels * info.frames);

		const size_t buffer_size = 256;
		double* buffer = new double[buffer_size];
		size_t total_bytes = 0;
		size_t read;
		while ((read = sf_read_double(file, buffer, buffer_size)) > 0) {
			copy_every_n(buffer, buffer + read * info.channels, data.begin() + total_bytes, info.channels);
			total_bytes += read;
		}
		delete[] buffer;
		sf_close(file);
	}
	else {		
		int err;
		mpg123_init();
		mpg123_handle* mh = mpg123_new(nullptr, &err);
		if (mpg123_open(mh, &name[0]) == MPG123_OK) {
			data.clear();
			long rate;
			int channels, encoding;
			mpg123_getformat(mh, &rate, &channels, &encoding);
			samplingRate = rate;
			auto samples = mpg123_length(mh);
			data.resize(samples);

			const size_t buffer_size = mpg123_outblock(mh);
			unsigned char* buffer = new unsigned char[buffer_size];
			short* ptr = reinterpret_cast<short*>(buffer);
			size_t total_bytes = 0;
			size_t done;
			size_t short_size = sizeof(short);
			size_t tmp = short_size * channels;
			for (; mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK;) {
				copy_every_n(ptr, ptr + done / short_size, data.begin() + total_bytes, channels);
				total_bytes += done / tmp;
			}
			delete buffer;
			mpg123_close(mh);
			mpg123_delete(mh);
			mpg123_exit();
		}
		else {
			printf("sndfile parsing error: %s\n", sf_strerror(file));
			return false;
		}
	}
	double min = *min_element(data.begin(), data.end());
	double max = *max_element(data.begin(), data.end());
	double norm = std::max(fabs(min), fabs(max));
	for (int i = 0; i < data.size(); i++) {
		data[i] /= norm;
	}
	return true;
}

