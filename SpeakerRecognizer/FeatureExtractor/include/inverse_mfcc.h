#pragma once

#include "mfcc.h"

class InverseMfcc : public Mfcc {
	virtual double MelScale(double freq) const override;
public:
	InverseMfcc();
};

