#include "inverse_mfcc.h"

InverseMfcc::InverseMfcc() : Mfcc() {
}

void InverseMfcc::InitFilterBanks() {
	const double lowFreq = sampleFreq / NFFT;
	f2 = sampleFreq / 2 + lowFreq;
	f1 = Mfcc::MelScale(maxFreq) + Mfcc::MelScale(lowFreq);
	Mfcc::InitFilterBanks();
}


double InverseMfcc::MelScale(double freq) const {
	return f1 - Mfcc::MelScale(f2 - freq);
}

