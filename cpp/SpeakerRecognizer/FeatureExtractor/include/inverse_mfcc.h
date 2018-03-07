#pragma once

#include "mfcc.h"

class InverseMfcc : public Mfcc {
	// precomputed imfcc coefficients
	double f1;
	double f2;
	
	double MelScale(double freq) const override;
	void InitFilterBanks() override;
public:
	InverseMfcc();
};

