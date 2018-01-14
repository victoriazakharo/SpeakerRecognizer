#pragma once

#include "mfcc.h"

class InverseMfcc : public Mfcc {
	double MelScale(double freq) const override;
public:
	InverseMfcc();
};

