#pragma once
#include <cmath>

const double DOUBLE_PI = 6.28318530717958647692;

double inline HammingWindow(const int k, const int n) {
	return 0.54 - 0.46 * cos(DOUBLE_PI * k / (n - 1));
}
