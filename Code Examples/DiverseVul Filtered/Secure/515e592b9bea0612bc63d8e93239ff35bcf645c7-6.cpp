static const char *subtypeString(int n) {
	if (n == 9) { // CPU_SUBTYPE_ARM_V7) {
		return "armv7";
	}
	return "?";
}