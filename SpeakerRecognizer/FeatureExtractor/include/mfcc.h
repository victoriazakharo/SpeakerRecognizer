#pragma once

#include <vector>

using std::vector;

class Mfcc {
protected:
	double* data;
	int noFilterBanks; 
	int NFFT; 
	double minFreq; 
	double maxFreq; 
	double sampleFreq;
	vector<vector<double>> filterBanks; 
	virtual double MelScale(double freq) const;
public:
	Mfcc();
	void Init(int noFilterBanks, int NFFT, double minFreq, double maxFreq, double sampleFreq);

	virtual ~Mfcc() {
	}

	void InitFilterBanks();
	void SetSpectrumData(double*);
	void GetLogCoefficents(vector<double>& v);
};
