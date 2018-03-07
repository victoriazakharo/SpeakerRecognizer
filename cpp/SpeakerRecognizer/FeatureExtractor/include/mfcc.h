#pragma once

#include <vector>

using std::vector;

class Mfcc {
protected:	
	int noFilterBanks; 
	int NFFT; 
	double minFreq; 
	double maxFreq; 
	double sampleFreq;
	vector<vector<double>> filterBanks; 
	virtual double MelScale(double freq) const;
	virtual void InitFilterBanks();
public:	
	void Init(int noFilterBanks, double windowSize, double sampleFreq);

	virtual ~Mfcc() = default;
	
	void GetLogCoefficents(const double* data, vector<double>& v);
};
