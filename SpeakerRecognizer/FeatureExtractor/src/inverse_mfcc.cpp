#include "inverse_mfcc.h"
#include <math.h>

InverseMfcc::InverseMfcc() : Mfcc() {
}

double InverseMfcc::MelScale(double freq) const {
	return 2195.286 - 2595 * log(1.0 + (8062.5 - freq) / 700.0);
}

